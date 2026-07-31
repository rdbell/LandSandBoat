#include "test_latent_party_member_plan_7524.h"

#include "map/latent_party_member_plan.h"

#include <cstdint>
#include <iostream>
#include <limits>

auto runLatentPartyMemberPlan7524SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent party-member plan 7524 self-test failed: " << label << '\n';
        }
        return value;
    };

    const auto expected = [](const xi::Latent condition, const std::uint16_t value, const std::size_t totalMembers, const int inZoneMembers) {
        switch (condition)
        {
            case xi::Latent::PartyMembers:
                return value <= totalMembers ? latenthelpers::PartyMemberLatentAction::Activate : latenthelpers::PartyMemberLatentAction::Deactivate;
            case xi::Latent::PartyMembersInZone:
                return value <= totalMembers && inZoneMembers == value ? latenthelpers::PartyMemberLatentAction::Activate : latenthelpers::PartyMemberLatentAction::Deactivate;
            default:
                return latenthelpers::PartyMemberLatentAction::Ignore;
        }
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        if (!expect(latenthelpers::DeterminePartyMemberLatentAction(condition, 2, 3, 2) == expected(condition, 2, 3, 2), "condition catalog"))
        {
            return false;
        }
    }

    const auto nativeMax = std::numeric_limits<std::size_t>::max();

    return expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembers, 3, 3, 0) == latenthelpers::PartyMemberLatentAction::Activate, "members reaches total") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembers, 4, 3, 0) == latenthelpers::PartyMemberLatentAction::Deactivate, "members above total") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembers, 3, nativeMax, 0) == latenthelpers::PartyMemberLatentAction::Activate, "members native size wrap") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembersInZone, 3, 3, 3) == latenthelpers::PartyMemberLatentAction::Activate, "in-zone exact total") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembersInZone, 4, 3, 4) == latenthelpers::PartyMemberLatentAction::Deactivate, "in-zone above total") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembersInZone, 3, nativeMax, 3) == latenthelpers::PartyMemberLatentAction::Activate, "in-zone native size wrap") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembersInZone, 2, 3, 3) == latenthelpers::PartyMemberLatentAction::Deactivate, "in-zone above value") &&
           expect(latenthelpers::DeterminePartyMemberLatentAction(xi::Latent::PartyMembersInZone, 2, 3, 1) == latenthelpers::PartyMemberLatentAction::Deactivate, "in-zone below value");
}
