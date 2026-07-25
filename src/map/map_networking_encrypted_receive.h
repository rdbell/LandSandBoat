#pragma once

#include "map_networking_capacity.h"

// Pure encrypted recv_parse outcome seam.
namespace mapnetworkingencryptedreceivehelpers
{

struct Plan
{
    mapnetworkinghelpers::IncomingDecryptionPlan decryption;
    int                                           decryptCount;
    bool                                          clearOutput;
    bool                                          markCurrentKeyDecryption;
};

// MakePlan composes recv_parse's primary/previous-key selection with its
// decrypt-count return convention, reject buffer clear, and current-key mark.
inline auto MakePlan(const bool primaryDecrypted, const bool pendingZone, const bool previousDecrypted) -> Plan
{
    const auto decryption = mapnetworkinghelpers::PlanIncomingDecryption(primaryDecrypted, pendingZone, previousDecrypted);
    switch (decryption)
    {
        case mapnetworkinghelpers::IncomingDecryptionPlan::UsePrimary:
            return Plan{
                .decryption                = decryption,
                .decryptCount              = 0,
                .clearOutput               = false,
                .markCurrentKeyDecryption  = mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption(0),
            };
        case mapnetworkinghelpers::IncomingDecryptionPlan::UsePrevious:
            return Plan{
                .decryption                = decryption,
                .decryptCount              = 1,
                .clearOutput               = false,
                .markCurrentKeyDecryption  = mapnetworkinghelpers::ShouldMarkCurrentKeyDecryption(1),
            };
        case mapnetworkinghelpers::IncomingDecryptionPlan::Reject:
            return Plan{
                .decryption                = decryption,
                .decryptCount              = -1,
                .clearOutput               = true,
                .markCurrentKeyDecryption  = false,
            };
    }

    return Plan{};
}

} // namespace mapnetworkingencryptedreceivehelpers
