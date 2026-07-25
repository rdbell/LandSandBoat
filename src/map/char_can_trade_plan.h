#pragma once
#include "common/cbasetypes.h"
namespace cantradeplanhelpers
{
struct Facts { bool charMonstro; bool targetMonstro; uint8 targetFreeSlots; uint8 tradeItemCount; bool rareDuplicate; };
enum class Decision : uint8 { RejectMonstrosity, RejectSpace, RejectRareDuplicate, Allow };
constexpr auto BuildPlan(const Facts facts) -> Decision
{
    if (facts.charMonstro || facts.targetMonstro) return Decision::RejectMonstrosity;
    if (facts.targetFreeSlots < facts.tradeItemCount) return Decision::RejectSpace;
    if (facts.rareDuplicate) return Decision::RejectRareDuplicate;
    return Decision::Allow;
}
} // namespace cantradeplanhelpers
