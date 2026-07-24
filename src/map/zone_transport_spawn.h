#pragma once

namespace zonetransportspawn
{

// ShouldSpawnForRecipient mirrors SpawnTransport's always-relevant filter.
constexpr auto ShouldSpawnForRecipient(const bool alwaysRelevant) -> bool
{
    return alwaysRelevant;
}

} // namespace zonetransportspawn
