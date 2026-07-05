/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_application.h"
#include "test_engine.h"

#include "common/settings.h"

#include <nlohmann/json.hpp>
#include <spdlog/async.h>

#include <future>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

constexpr auto TestDaemonPrefix = "XI_TESTD ";

auto jsonStringArray(const nlohmann::json& object, const char* key) -> std::vector<std::string>
{
    std::vector<std::string> values;
    if (!object.contains(key))
    {
        return values;
    }

    const auto& node = object.at(key);
    if (node.is_string())
    {
        values.push_back(node.get<std::string>());
        return values;
    }

    if (!node.is_array())
    {
        return values;
    }

    for (const auto& value : node)
    {
        if (value.is_string())
        {
            values.push_back(value.get<std::string>());
        }
    }

    return values;
}

void writeDaemonResponse(nlohmann::json response)
{
    std::cout << TestDaemonPrefix << response.dump() << std::endl;
}

auto daemonReportSummary(const std::string& outputPath) -> nlohmann::json
{
    if (outputPath.empty())
    {
        return nullptr;
    }

    std::ifstream file(outputPath);
    if (!file.is_open())
    {
        return nullptr;
    }

    try
    {
        nlohmann::json report;
        file >> report;
        if (report.contains("results") && report["results"].contains("summary"))
        {
            return report["results"]["summary"];
        }
    }
    catch (...)
    {
    }

    return nullptr;
}

auto appConfig() -> ApplicationConfig
{
    return ApplicationConfig{
        .serverName = "test",
        .arguments  = {
            ArgumentDefinition{
                .name        = "--keep-going",
                .description = "Continue as much as possible after an error or failure.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--verbose",
                .description = "Verbose output of errors.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--watch",
                .description = "Watch files for changes and re-run tests. (Not yet implemented)",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--no-lua-smoke",
                .description = "Skip loading every non-test Lua file during test-server startup.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--daemon-stdio",
                .description = "Keep the warmed test server running and accept JSON-lines commands on stdin.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--tag",
                .description = "Only run tests with these #tags.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--no-tag",
                .description = "Do not run tests with these #tags, takes precedence over tags.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--file",
                .description = "Only run test files matching the regex pattern.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--no-file",
                .description = "Do not run test files matching the regex pattern, takes precedence over file.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--filter",
                .description = "Only run test names matching the regex pattern.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--no-filter",
                .description = "Do not run test names matching the regex pattern, takes precedence over filter.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--output",
                .description = "Output file for test results. Use .json extension for CTRF format.",
            },
        },
    };
}

} // namespace

TestApplication::TestApplication(const int argc, char** argv)
: Application(appConfig(), argc, argv)
, sink_(std::make_shared<InMemorySink>())
{
}

TestApplication::~TestApplication() = default;

auto TestApplication::createEngine() -> std::unique_ptr<Engine>
{
    // Required by Application, but initialized manually in run()
    return nullptr;
}

auto TestApplication::run() -> bool
{
    TracyZoneScoped;
    const bool daemonMode = args().get<bool>("--daemon-stdio");

    scheduler_.postToMainThread(
        [&, daemonMode]() -> Task<void>
        {
            // The test harness embeds both the world and map servers in this one process. Route their
            // IPC over inproc:// (a shared, in-process transport) instead of a TCP port.
            settings::set("network.ZMQ_TRANSPORT", std::string("inproc"));

            //
            // Prepare WorldEngine
            //

            auto worldEngine = std::make_unique<WorldEngine>(scheduler_, zmqService_, WorldEngine::EnableHTTPServer::No);

            worldEngine->onInitialize();

            //
            // Prepare MapEngine
            //

            MapConfig mapConfig{
                .inCI              = true,
                .isTestServer      = true,
                .lazyZones         = true,
                .controlledWeather = true,
                .smokeLuaFiles     = !args().get<bool>("--no-lua-smoke"),
            };
            auto mapEngine = std::make_unique<MapEngine>(*this, mapConfig);

            // We must ensure that mapEngine->init() is complete before we
            // try and construct TestEngine or run the tests
            scheduler_.blockOnMainThread(mapEngine->init());

            mapEngine->onInitialize();

            //
            // Prepare TestEngine with MapEngine and WorldEngine
            //

            auto testConfig = baseTestConfig();

            engine_ = std::make_unique<TestEngine>(*this, std::move(testConfig), std::move(mapEngine), std::move(worldEngine));

            // From this point, every logging statements end up in the in-memory sink
            // Print to stderr directly if needed
            captureLogger();

            if (daemonMode)
            {
                daemonThread_ = std::thread(&TestApplication::runDaemonStdio, this);
                co_return;
            }

            // Record the result and exit through the normal path so main() can run
            // lua_cleanup() before the process tears down.
            success_ = co_await static_cast<TestEngine*>(engine_.get())->executeTests();
            this->requestExit();
        });

    try
    {
        scheduler_.run(); // blocks
    }
    catch (const std::exception& e)
    {
        ShowCriticalFmt("Fatal Exception: {}", e.what());
        success_ = false;
    }

    if (daemonThread_.joinable())
    {
        daemonThread_.join();
    }

    return success_;
}

auto TestApplication::baseTestConfig() const -> TestConfig
{
    return TestConfig{
        .loggerSink = sink_,
        .verbose    = args().get<bool>("--verbose"),
        .output     = args().present<std::string>("--output").value_or(""),
        .keepGoing  = args().get<bool>("--keep-going"),
        .watch      = args().get<bool>("--watch"),
        .console    = true,
        .filters    = {
            .includePatterns = args().get<std::vector<std::string>>("--file"),
            .excludePatterns = args().get<std::vector<std::string>>("--no-file"),
            .includeFilters  = args().get<std::vector<std::string>>("--filter"),
            .excludeFilters  = args().get<std::vector<std::string>>("--no-filter"),
            .includeTags     = args().get<std::vector<std::string>>("--tag"),
            .excludeTags     = args().get<std::vector<std::string>>("--no-tag"),
        },
    };
}

void TestApplication::runDaemonStdio()
{
    success_ = true;
    writeDaemonResponse({
        { "event", "ready" },
        { "protocol", 1 },
    });

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            continue;
        }

        nlohmann::json command;
        try
        {
            command = nlohmann::json::parse(line);
        }
        catch (const std::exception& e)
        {
            writeDaemonResponse({
                { "status", "error" },
                { "error", std::format("invalid JSON command: {}", e.what()) },
            });
            continue;
        }

        const auto id = command.value("id", std::string{});
        const auto op = command.value("op", std::string{});

        if (op == "shutdown")
        {
            success_ = true;
            writeDaemonResponse({
                { "id", id },
                { "status", "ok" },
            });
            requestExit();
            break;
        }

        if (op != "run")
        {
            writeDaemonResponse({
                { "id", id },
                { "status", "error" },
                { "error", std::format("unknown daemon op '{}'", op) },
            });
            continue;
        }

        auto testConfig                  = baseTestConfig();
        testConfig.output                = command.value("output", testConfig.output);
        testConfig.keepGoing             = command.value("keepGoing", testConfig.keepGoing);
        testConfig.verbose               = command.value("verbose", testConfig.verbose);
        testConfig.console               = command.value("console", false);
        testConfig.watch                 = false;
        if (command.contains("files"))
        {
            testConfig.filters.includePatterns = jsonStringArray(command, "files");
        }
        if (command.contains("excludeFiles"))
        {
            testConfig.filters.excludePatterns = jsonStringArray(command, "excludeFiles");
        }
        if (command.contains("filters"))
        {
            testConfig.filters.includeFilters = jsonStringArray(command, "filters");
        }
        if (command.contains("excludeFilters"))
        {
            testConfig.filters.excludeFilters = jsonStringArray(command, "excludeFilters");
        }
        if (command.contains("tags"))
        {
            testConfig.filters.includeTags = jsonStringArray(command, "tags");
        }
        if (command.contains("excludeTags"))
        {
            testConfig.filters.excludeTags = jsonStringArray(command, "excludeTags");
        }
        const auto outputPath = testConfig.output;

        auto promise = std::make_shared<std::promise<bool>>();
        auto future  = promise->get_future();

        scheduler_.postToMainThread(
            [this, promise, testConfig = std::move(testConfig)]() mutable -> Task<void>
            {
                try
                {
                    const auto ok = co_await static_cast<TestEngine*>(engine_.get())->executeTests(std::move(testConfig));
                    promise->set_value(ok);
                }
                catch (...)
                {
                    promise->set_exception(std::current_exception());
                }
            });

        try
        {
            const auto ok = future.get();
            auto response = nlohmann::json{
                { "id", id },
                { "status", ok ? "passed" : "failed" },
                { "ok", ok },
            };
            if (auto summary = daemonReportSummary(outputPath); !summary.is_null())
            {
                response["summary"] = std::move(summary);
            }
            writeDaemonResponse(std::move(response));
            success_ = success_ && ok;
        }
        catch (const std::exception& e)
        {
            writeDaemonResponse({
                { "id", id },
                { "status", "error" },
                { "error", e.what() },
            });
            success_ = false;
        }
    }

    requestExit();
}

// Replace all loggers sinks with the in-memory sink
void TestApplication::captureLogger() const
{
    const auto loggerNames = std::vector<std::string>{
        "critical", "error", "lua", "warn", "info", "debug", "trace"
    };

    // spdlog blows up if we don't!
    spdlog::shutdown();

    // Re-register all loggers as SYNCHRONOUS loggers with only our in-memory sink
    // This ensures all logs are immediately written to the sink
    for (const auto& name : loggerNames)
    {
        const auto logger = std::make_shared<spdlog::logger>(name, sink_);
        logger->set_level(spdlog::level::trace);
        spdlog::register_logger(logger);
    }

    // The loggers were just dropped and re-created, so the lock-free loggerFor cache now holds
    // dangling pointers. Re-resolve it before anything logs again.
    logging::RefreshLoggerCache();

    logging::SetPattern(settings::get<std::string>("test.PATTERN"));
}
