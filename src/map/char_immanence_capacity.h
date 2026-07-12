#pragma once

#include <cstdint>
#include <optional>

// Pure Immanence spell-family → skillchain-element mapping from
// CCharEntity::OnCastFinished. Host still runs GetSkillChainEffect/damage.

namespace charimmanencehelpers
{

// SPELLFAMILY pins used by Immanence switch.
constexpr std::uint16_t FamilyFire        = 42;
constexpr std::uint16_t FamilyBlizzard    = 43;
constexpr std::uint16_t FamilyAero        = 44;
constexpr std::uint16_t FamilyStone       = 45;
constexpr std::uint16_t FamilyThunder     = 46;
constexpr std::uint16_t FamilyWater       = 47;
constexpr std::uint16_t FamilyGeohelix    = 76;
constexpr std::uint16_t FamilyHydrohelix  = 77;
constexpr std::uint16_t FamilyAnemohelix  = 78;
constexpr std::uint16_t FamilyPyrohelix   = 79;
constexpr std::uint16_t FamilyCryohelix   = 80;
constexpr std::uint16_t FamilyIonohelix   = 81;
constexpr std::uint16_t FamilyNoctohelix  = 82;
constexpr std::uint16_t FamilyLuminohelix = 83;

// SKILLCHAIN_ELEMENT pins.
constexpr std::uint8_t SCTransfixion   = 1;
constexpr std::uint8_t SCCompression   = 2;
constexpr std::uint8_t SCLiquefaction  = 3;
constexpr std::uint8_t SCScission      = 4;
constexpr std::uint8_t SCReverberation = 5;
constexpr std::uint8_t SCDetonation    = 6;
constexpr std::uint8_t SCInduration    = 7;
constexpr std::uint8_t SCImpaction     = 8;

struct Mapping
{
    bool         applies{}; // false for default branch (do not remove Immanence)
    bool         isHelix{};
    std::uint8_t skillchainElement{}; // valid when applies
};

// MapFamily returns Immanence SC mapping for a black-magic spell family.
constexpr auto MapFamily(const std::uint16_t spellFamily) -> Mapping
{
    switch (spellFamily)
    {
        case FamilyGeohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCScission };
        case FamilyStone:
            return Mapping{ .applies = true, .isHelix = false, .skillchainElement = SCScission };
        case FamilyHydrohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCReverberation };
        case FamilyWater:
            return Mapping{ .applies = true, .isHelix = false, .skillchainElement = SCReverberation };
        case FamilyAnemohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCDetonation };
        case FamilyAero:
            return Mapping{ .applies = true, .isHelix = false, .skillchainElement = SCDetonation };
        case FamilyPyrohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCLiquefaction };
        case FamilyFire:
            return Mapping{ .applies = true, .isHelix = false, .skillchainElement = SCLiquefaction };
        case FamilyCryohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCInduration };
        case FamilyBlizzard:
            return Mapping{ .applies = true, .isHelix = false, .skillchainElement = SCInduration };
        case FamilyIonohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCImpaction };
        case FamilyThunder:
            return Mapping{ .applies = true, .isHelix = false, .skillchainElement = SCImpaction };
        case FamilyNoctohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCCompression };
        case FamilyLuminohelix:
            return Mapping{ .applies = true, .isHelix = true, .skillchainElement = SCTransfixion };
        default:
            return Mapping{};
    }
}

// HelixDurationExtensionSeconds is the +2s skillchain window for helix closes.
constexpr auto HelixDurationExtensionSeconds() -> std::int64_t
{
    return 2;
}

} // namespace charimmanencehelpers
