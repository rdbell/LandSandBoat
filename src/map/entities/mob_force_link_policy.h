#pragma once
namespace mobforcelinkhelpers
{
inline auto ShouldForceLink(bool dynamis, bool battlefield, bool superLink) -> bool { return dynamis || battlefield || superLink; }
}
