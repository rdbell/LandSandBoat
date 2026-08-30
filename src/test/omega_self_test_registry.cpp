#include "omega_self_test_registry.h"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <utility>

namespace omega::selftest
{
namespace
{

struct Entry
{
    std::string  name;
    TestFunction function;
};

auto entries() -> std::vector<Entry>&
{
    static std::vector<Entry> value;
    return value;
}

auto entriesMutex() -> std::mutex&
{
    static std::mutex value;
    return value;
}

} // namespace

auto Register(const char* name, TestFunction function) -> bool
{
    if (name == nullptr || *name == '\0' || function == nullptr)
    {
        throw std::invalid_argument("Omega self-test registration requires a name and function");
    }

    std::lock_guard lock(entriesMutex());
    const auto      duplicate = std::find_if(entries().begin(), entries().end(), [name](const Entry& entry)
                                             {
                                            return entry.name == name;
                                             });
    if (duplicate != entries().end())
    {
        throw std::invalid_argument("Duplicate Omega self-test registration: " + std::string(name));
    }
    entries().push_back({ name, function });
    return true;
}

auto Run(const std::vector<std::string>& filters) -> bool
{
    std::vector<std::regex> patterns;
    patterns.reserve(filters.size());
    try
    {
        for (const auto& filter : filters)
        {
            patterns.emplace_back(filter, std::regex::ECMAScript | std::regex::icase);
        }
    }
    catch (const std::regex_error& error)
    {
        std::cerr << "Invalid Omega self-test filter: " << error.what() << '\n';
        return false;
    }

    std::vector<Entry> registered;
    {
        std::lock_guard lock(entriesMutex());
        registered = entries();
    }
    std::sort(registered.begin(), registered.end(), [](const Entry& left, const Entry& right)
              {
                  return left.name < right.name;
              });

    bool matched = patterns.empty();
    bool ok      = true;
    for (const auto& entry : registered)
    {
        const auto selected = patterns.empty() || std::any_of(patterns.begin(), patterns.end(), [&entry](const std::regex& pattern)
                                                              {
                                                                  return std::regex_search(entry.name, pattern);
                                                              });
        if (!selected)
        {
            continue;
        }

        matched = true;
        std::cout << "Running Omega self-test " << entry.name << '\n';
        ok = entry.function() && ok;
    }

    if (!matched)
    {
        std::cerr << "Omega self-test filters matched no registered tests\n";
        return false;
    }
    return ok;
}

} // namespace omega::selftest
