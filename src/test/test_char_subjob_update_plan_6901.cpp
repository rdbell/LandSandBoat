#include "test_char_subjob_update_plan_6901.h"

#include "map/char_subjob_update_plan.h"

#include <iostream>

auto runCharSubJobUpdatePlan6901SelfTests() -> bool
{
    using Action = subjobupdatehelpers::Action;

    const bool ok = subjobupdatehelpers::BuildPlan() == std::array{
                                                        Action::RefreshGiftMods,
                                                        Action::BuildSkills,
                                                        Action::CalculateStats,
                                                        Action::CheckValidEquipment,
                                                        Action::ChangeRecasts,
                                                        Action::BuildAbilities,
                                                        Action::BuildTraits,
                                                        Action::UpdateHealth,
                                                        Action::RestoreHP,
                                                        Action::RestoreMP,
                                                        Action::SaveStats,
                                                        Action::SaveJob,
                                                        Action::SaveExperience,
                                                        Action::MarkHealthUpdate,
                                                        Action::PacketJobInfo,
                                                        Action::PacketCliStatus,
                                                        Action::PacketCliStatus2,
                                                        Action::PacketAbilityRecast,
                                                        Action::PacketCommandData,
                                                        Action::PacketCharacterStatus,
                                                        Action::PacketMerits,
                                                        Action::PacketMonstrosity1,
                                                        Action::PacketMonstrosity2,
                                                        Action::PacketCharacterSync,
                                                    };
    if (!ok)
    {
        std::cerr << "subjob update plan 6901 self-test failed\n";
    }
    return ok;
}
