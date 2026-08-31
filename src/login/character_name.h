#pragma once

#include "common/utils.h"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <common/types/maybe.h>

// Pure character-name validation helpers for VIEW 0x22 (name check) extracted
// so native tests can pin policy without sockets/SQL/Lua hosts.

namespace loginHelpers
{

// Character names are copied from a PacketNameLength (16) field with the last
// byte reserved as a NUL terminator (15 content bytes max on the wire).
constexpr std::size_t CharacterNameFieldBytes = PacketNameLength - 1;
constexpr std::size_t CharacterNameMinLength  = 3;
constexpr std::size_t CharacterNameMaxLength  = 15;

// Fixed local invalid-name reason strings from view_session 0x22.
inline constexpr const char* CharacterNameInvalidCharactersReason = "Invalid characters present in name.";
inline constexpr const char* CharacterNameInvalidLengthReason     = "Invalid name length.";
inline constexpr const char* CharacterNameAlreadyInUseReason      = "Name already in use.";
inline constexpr const char* CharacterNameEntityQueryFailedReason = "Internal entity name query failed.";
// Mob/NPC query failure omits the trailing period (LSB literal parity).
inline constexpr const char* CharacterNameEntityQueryFailedNoPeriodReason = "Internal entity name query failed";

// character_creation_gate is the pure outcome of MAINT_MODE / CHARACTER_CREATION
// before name validation on VIEW 0x22.
enum class character_creation_gate : uint8_t
{
    ALLOW,
    DENIED, // maintenance or creation disabled
};

// ClassifyCharacterCreationGate mirrors maintMode > 0 || !enableCharacterCreation.
inline auto ClassifyCharacterCreationGate(const uint8 maintMode, const bool enableCharacterCreation) -> character_creation_gate
{
    if (maintMode > 0 || !enableCharacterCreation)
    {
        return character_creation_gate::DENIED;
    }
    return character_creation_gate::ALLOW;
}

// ExtractCharacterNameField builds a std::string from up to 15 packet bytes as
// a C string (stops at the first embedded NUL).
inline auto ExtractCharacterNameField(const char* field, const std::size_t maxBytes = CharacterNameFieldBytes) -> std::string
{
    if (field == nullptr || maxBytes == 0)
    {
        return {};
    }
    return std::string(field); // field is expected NUL-terminated within maxBytes+1
}

// IsAlphabeticCharacterName reports whether every byte is std::isalpha under the
// unsigned-char cast used for defined isalpha behavior on high bytes.
inline auto IsAlphabeticCharacterName(const std::string& name) -> bool
{
    for (const unsigned char ch : name)
    {
        if (!std::isalpha(ch))
        {
            return false;
        }
    }
    return true;
}

// IsValidCharacterNameLength mirrors size in [3, 15].
inline auto IsValidCharacterNameLength(const std::string& name) -> bool
{
    return name.size() >= CharacterNameMinLength && name.size() <= CharacterNameMaxLength;
}

// ValidateCharacterNameLocal applies alphabetic then length checks with LSB's
// overwrite semantics: a length failure replaces an alphabetic failure reason.
// Returns nullopt when local checks pass (DB/banned-word still host-side).
inline auto ValidateCharacterNameLocal(const std::string& name) -> Maybe<std::string>
{
    Maybe<std::string> reason = std::nullopt;

    for (const unsigned char ch : name)
    {
        if (!std::isalpha(ch))
        {
            reason = CharacterNameInvalidCharactersReason;
            break;
        }
    }

    if (name.size() < CharacterNameMinLength || name.size() > CharacterNameMaxLength)
    {
        reason = CharacterNameInvalidLengthReason;
    }

    return reason;
}

// FormatBannedWordMatchReason mirrors the 0x22 bad-words diagnostic. The word
// embedded in the message is already uppercased by production before format.
inline auto FormatBannedWordMatchReason(const std::string& upperBadWord) -> std::string
{
    return fmt::format("Name matched with bad words list <{}>.", upperBadWord);
}

// FindBannedWordMatch scans bad words (already uppercased entries) for a
// substring of the uppercased name. Later matches overwrite earlier ones,
// matching the view_session loop.
inline auto FindBannedWordMatch(const std::string& upperName, const std::vector<std::string>& upperBadWords) -> Maybe<std::string>
{
    Maybe<std::string> reason = std::nullopt;
    for (const auto& badWord : upperBadWords)
    {
        if (upperName.find(badWord) != std::string::npos)
        {
            reason = FormatBannedWordMatchReason(badWord);
        }
    }
    return reason;
}

// ResolveCharacterNameInvalidReason mirrors VIEW 0x22's ordered validation
// overwrites after local checks. Every later host query or banned-word match
// replaces an earlier reason, including a successful local validation.
inline auto ResolveCharacterNameInvalidReason(
    Maybe<std::string> localReason,
    const bool        entityQueryOk,
    const bool        entityNameTaken,
    const bool        checkMobNPCNames,
    const bool        mobNPCQueryOk,
    const bool        mobNPCNameTaken,
    Maybe<std::string> bannedReason) -> Maybe<std::string>
{
    auto reason = std::move(localReason);
    if (!entityQueryOk)
    {
        reason = CharacterNameEntityQueryFailedReason;
    }
    else if (entityNameTaken)
    {
        reason = CharacterNameAlreadyInUseReason;
    }

    if (checkMobNPCNames)
    {
        if (!mobNPCQueryOk)
        {
            reason = CharacterNameEntityQueryFailedNoPeriodReason;
        }
        else if (mobNPCNameTaken)
        {
            reason = CharacterNameAlreadyInUseReason;
        }
    }

    if (bannedReason)
    {
        reason = std::move(bannedReason);
    }
    return reason;
}

} // namespace loginHelpers
