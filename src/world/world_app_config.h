#pragma once

// Pure WorldApplication configuration constants extracted so native tests can
// pin registration without Application/scheduler hosts.

namespace worldapp
{

// WorldServerName is ApplicationConfig.serverName for WorldApplication.
inline constexpr const char* WorldServerName = "world";

// HTTP server enable setting key used when constructing WorldEngine.
// Host reads settings::get<bool>(WorldHTTPEnableSettingKey).
inline constexpr const char* WorldHTTPEnableSettingKey = "network.ENABLE_HTTP";

// WorldMainExitCode is the fixed process exit code after worldApp->run().
// Production always returns 0 regardless of run outcome.
// Go host pure half: worldapp.WorldMain always returns this (slice 6373).
inline constexpr int WorldMainExitCode = 0;

} // namespace worldapp
