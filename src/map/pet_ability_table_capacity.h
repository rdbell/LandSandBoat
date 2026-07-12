#pragma once

#include <cstdint>

// Pure BuildingCharPetAbilityTable and BuildingCharTraitsTable policy.

namespace petabilitytablehelpers
{

// Job / pet / ability pins.
constexpr std::uint8_t  JobSMN             = 15;
constexpr std::uint32_t PetIDCarbuncle      = 8;
constexpr std::uint32_t PetIDFenrir        = 9;
constexpr std::uint32_t PetIDRamuh         = 15;
constexpr std::uint32_t PetIDDiabolos      = 16;
constexpr std::uint32_t PetIDCaitSith      = 20;
constexpr std::uint32_t PetIDSiren         = 76;

constexpr std::uint16_t AbilityHealingRuby    = 512;
constexpr std::uint16_t AbilitySoothingRuby   = 520;
constexpr std::uint16_t AbilityMoonlitCharge  = 528;
constexpr std::uint16_t AbilityCamisado       = 656;
constexpr std::uint16_t AbilityPerfectDefense = 671;
constexpr std::uint16_t AbilityPacifyingRuby  = 773;
constexpr std::uint16_t AbilityClarsachCall   = 960;
constexpr std::uint16_t AbilityHystericAssault = 970;

constexpr std::uint16_t PacifyingRubyPacketBit = 261;
constexpr std::uint16_t SirenAbilityPacketOffset = 0x1C0;

// ShouldRejectNullPetOrChar mirrors PPet == nullptr || PChar == nullptr.
constexpr auto ShouldRejectNullPetOrChar(const bool petNull, const bool charNull) -> bool
{
    return petNull || charNull;
}

// ShouldClearPetCommandsOnly mirrors PetID == 0.
constexpr auto ShouldClearPetCommandsOnly(const std::uint32_t petID) -> bool
{
    return petID == 0;
}

// IsSummonerJob mirrors mjob/sjob == SMN.
constexpr auto IsSummonerJob(const std::uint8_t mJob, const std::uint8_t sJob) -> bool
{
    return mJob == JobSMN || sJob == JobSMN;
}

// IsSpiritOrSirenPet mirrors carbuncle..cait_sith range or siren.
constexpr auto IsSpiritOrSirenPet(const std::uint32_t petID) -> bool
{
    return (petID >= PetIDCarbuncle && petID <= PetIDCaitSith) || petID == PetIDSiren;
}

// ShouldConsiderSMNPetAbility mirrors petLevel >= abilityLevel && spirit/siren && addtypeOK.
constexpr auto ShouldConsiderSMNPetAbility(const std::uint8_t petLevel, const std::uint8_t abilityLevel, const std::uint32_t petID, const bool addtypeOK) -> bool
{
    return petLevel >= abilityLevel && IsSpiritOrSirenPet(petID) && addtypeOK;
}

// Carbuncle ability bit mapping.
constexpr auto IsCarbuncleAbility(const std::uint16_t abilityID) -> bool
{
    return (abilityID >= AbilityHealingRuby && abilityID <= AbilitySoothingRuby) || abilityID == AbilityPacifyingRuby;
}

constexpr auto CarbunclePetAbilityBit(const std::uint16_t abilityID) -> std::uint16_t
{
    if (abilityID == AbilityPacifyingRuby)
    {
        return PacifyingRubyPacketBit;
    }
    return static_cast<std::uint16_t>(abilityID - AbilityHealingRuby);
}

// Elemental avatar range: FENRIR..RAMUH with ability band HEALING_RUBY + (petID-8)*16
constexpr auto IsElementalAvatarPet(const std::uint32_t petID) -> bool
{
    return petID >= PetIDFenrir && petID <= PetIDRamuh;
}

constexpr auto ElementalAvatarAbilityInBand(const std::uint16_t abilityID, const std::uint32_t petID) -> bool
{
    const auto lo = static_cast<std::uint16_t>(AbilityHealingRuby + ((petID - 8) * 16));
    const auto hi = static_cast<std::uint16_t>(AbilityHealingRuby + ((petID - 7) * 16));
    return abilityID >= lo && abilityID < hi;
}

constexpr auto AvatarPetAbilityBit(const std::uint16_t abilityID) -> std::uint16_t
{
    return static_cast<std::uint16_t>(abilityID - AbilityHealingRuby);
}

constexpr auto IsDiabolosAbility(const std::uint16_t abilityID) -> bool
{
    return abilityID >= AbilityCamisado && abilityID <= AbilityPerfectDefense;
}

constexpr auto IsCaitSithAbility(const std::uint16_t abilityID) -> bool
{
    return abilityID > AbilitySoothingRuby && abilityID < AbilityMoonlitCharge;
}

constexpr auto IsSirenAbility(const std::uint16_t abilityID) -> bool
{
    return abilityID >= AbilityClarsachCall && abilityID <= AbilityHystericAssault;
}

constexpr auto SirenPetAbilityBit(const std::uint16_t abilityID) -> std::uint16_t
{
    return static_cast<std::uint16_t>((abilityID - AbilityClarsachCall) + SirenAbilityPacketOffset);
}

// Jug pet skill list: abilityid - HEALING_RUBY
constexpr auto JugPetAbilityBit(const std::uint16_t abilityID) -> std::uint16_t
{
    return static_cast<std::uint16_t>(abilityID - AbilityHealingRuby);
}

// --- Traits table ---

// ShouldApplyMonstrosityJobs mirrors monstro non-null (host applies job override).
constexpr auto ShouldApplyMonstrosityJobs(const bool hasMonstrosity) -> bool
{
    return hasMonstrosity;
}

// ShouldCalculateBlueTraits mirrors mjob/sjob == BLU (JOB_BLU = 16).
constexpr std::uint8_t JobBLU = 16;

constexpr auto ShouldCalculateBlueTraits(const std::uint8_t mJob, const std::uint8_t sJob) -> bool
{
    return mJob == JobBLU || sJob == JobBLU;
}

// PlayerMEVARankG skill rank pin used for GetMaxSkill(12, mlvl).
constexpr std::uint8_t PlayerMEVASkillRank = 12;

} // namespace petabilitytablehelpers
