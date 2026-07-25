-----------------------------------
-- Avatars Favor helper
-- Dual-wired pure inject forms (slice 6725 / 0874):
--   shouldAvatarsFavorBeApplied, lookupAvatarsFavor, scalingFromParams,
--   clampPowerToSkillFromParams, tickPowerFromParams, preSOADebuffs
-- Parity: internal/avatarsfavor
-----------------------------------
require('scripts/globals/pets')
-----------------------------------
xi = xi or {}
xi.avatarsFavor = xi.avatarsFavor or {}

-- Pins matching internal/avatarsfavor.
xi.avatarsFavor.maxSkillRank  = 7
xi.avatarsFavor.maxTickPower  = 11
xi.avatarsFavor.overSkillCap  = 670
xi.avatarsFavor.preSOADebuffMATT = 20
xi.avatarsFavor.preSOADebuffATTP = 20
xi.avatarsFavor.preSOADebuffACC  = 10
xi.avatarsFavor.preSOADebuffDEFP = 10

xi.avatarsFavor.skillLevels =
{
    [1] = 316,
    [2] = 381,
    [3] = 446,
    [4] = 511,
    [5] = 574,
    [6] = 669,
    [7] = 670,
}

-- Based on BGWiki Power 8 -> 11 is only available with +Avatars Favor Gear + Job Point Gift
-- Avatar Favor buffs scale per tick (~9 seconds) to a max value based on current summoning skill
local avatarsFavorEffect =
{
    [xi.petId.CARBUNCLE] = -- Regen
    {
        scaling = { 12, 14, 16, 18, 20, 21, 24, 26, 27, 28, 29 },
        effect = xi.effect.CARBUNCLES_FAVOR
    },

    [xi.petId.FENRIR] = -- Magic Eva
    {
        scaling = { 1, 2, 3, 4, 5, 7, 9, 12, 15, 18, 21 },
        effect = xi.effect.FENRIRS_FAVOR
    },

    [xi.petId.IFRIT] = -- Double Attack
    {
        scaling = { 12, 12, 15, 15, 23, 23, 24, 24, 25, 25, 26 },
        effect = xi.effect.IFRITS_FAVOR
    },

    [xi.petId.TITAN] = -- Defense
    {
        scaling = { 57, 62, 67, 72, 77, 82, 87, 92, 97, 102, 107 },
        effect = xi.effect.TITANS_FAVOR
    },

    [xi.petId.LEVIATHAN] = -- Magic Accuracy
    {
        scaling = { 1, 2, 3, 4, 5, 7, 9, 12, 15, 18, 21 },
        effect = xi.effect.LEVIATHANS_FAVOR
    },

    [xi.petId.GARUDA] = -- Evasion
    {
        scaling = { 12, 15, 18, 22, 25, 28, 31, 34, 37, 40, 43 },
        effect = xi.effect.GARUDAS_FAVOR
    },

    [xi.petId.SHIVA] = -- Magic Attack
    {
        scaling = { 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45 },
        effect = xi.effect.SHIVAS_FAVOR
    },

    [xi.petId.RAMUH] = -- Potency (Critical Hit+%)
    {
        scaling = { 12, 14, 16, 18, 19, 21, 21, 23, 23, 24, 24 },
        effect = xi.effect.RAMUHS_FAVOR
    },

    [xi.petId.DIABOLOS] = -- Refresh
    {
        scaling = { 3, 4, 4, 5, 5, 5, 6, 7, 7, 8, 8 },
        effect = xi.effect.DIABOLOSS_FAVOR
    },

    [xi.petId.CAIT_SITH] = -- Magic Defense
    {
        scaling = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 27, 28 },
        effect = xi.effect.CAIT_SITHS_FAVOR
    },

    [xi.petId.SIREN] = -- Subtle Blow
    {
        scaling = { 9, 11, 13, 15, 17, 19, 20, 21, 22, 23 },
        effect = xi.effect.SIRENS_FAVOR
    },
}

-----------------------------------
-- Given a :getPetID petID (Not a getMobID)
-- Returns if Avatars Favor should be applied
-- Dual-wired to internal/avatarsfavor.ShouldApply
-----------------------------------
xi.avatarsFavor.shouldAvatarsFavorBeApplied = function(petId)
    local shouldApply = false

    if petId and petId >= xi.petId.CARBUNCLE and petId <= xi.petId.DIABOLOS then
        shouldApply = true
    end

    if petId and (petId == xi.petId.CAIT_SITH or petId == xi.petId.SIREN) then
        shouldApply = true
    end

    return shouldApply
end

-- Pure catalog lookup (internal/avatarsfavor.Lookup).
-- returns: { effect, scaling } or nil
xi.avatarsFavor.lookupAvatarsFavor = function(petId)
    local row = avatarsFavorEffect[petId]
    if not row then
        return nil
    end

    return {
        effect  = row.effect,
        scaling = row.scaling,
    }
end

-- Pure scaling ladder index (internal/avatarsfavor.Scaling).
-- params: petId, power (1-based favor rank)
-- returns: subPower or nil
xi.avatarsFavor.scalingFromParams = function(params)
    params = params or {}
    local row = avatarsFavorEffect[params.petId]
    if not row then
        return nil
    end

    local power = params.power or 0
    if power < 1 or power > #row.scaling then
        return nil
    end

    return row.scaling[power]
end

-- Pure skill-cap half of onEffectTick (internal/avatarsfavor.ClampPowerToSkill).
-- params: power, summoningSkill
xi.avatarsFavor.clampPowerToSkillFromParams = function(params)
    params = params or {}
    local power          = params.power or 0
    local summoningSkill = params.summoningSkill or 0

    for i = 1, xi.avatarsFavor.maxSkillRank do
        if
            summoningSkill <= xi.avatarsFavor.skillLevels[i] and
            power > i
        then
            return i
        elseif
            summoningSkill > xi.avatarsFavor.overSkillCap and
            power > xi.avatarsFavor.maxSkillRank
        then
            power = xi.avatarsFavor.maxSkillRank
        end
    end

    return power
end

-- Pure onEffectTick power mutation (internal/avatarsfavor.TickPower).
-- params: currentPower, summoningSkill, gearEnhance
xi.avatarsFavor.tickPowerFromParams = function(params)
    params = params or {}
    local power = params.currentPower or 0

    if power <= xi.avatarsFavor.maxTickPower then
        power = power + 1
    end

    power = xi.avatarsFavor.clampPowerToSkillFromParams({
        power          = power,
        summoningSkill = params.summoningSkill or 0,
    })

    return power + (params.gearEnhance or 0)
end

-- Pre-SoA pet debuff magnitudes (internal/avatarsfavor.PreSOADebuffs).
-- returns: matt, attp, acc, defp
xi.avatarsFavor.preSOADebuffs = function()
    return xi.avatarsFavor.preSOADebuffMATT,
        xi.avatarsFavor.preSOADebuffATTP,
        xi.avatarsFavor.preSOADebuffACC,
        xi.avatarsFavor.preSOADebuffDEFP
end

-- Catalog size (11 avatars).
xi.avatarsFavor.catalogSize = function()
    local n = 0
    for _ in pairs(avatarsFavorEffect) do
        n = n + 1
    end

    return n
end

local removeAvatarsFavorDebuffsFromPet = function(target)
    local pet = target:getPet()
    if pet then
        local petId = pet:getPetID()
        if  -- Different pet states for in and out of retail / eras
            xi.avatarsFavor.shouldAvatarsFavorBeApplied(petId) and
            xi.settings.main.ENABLE_SOA == 0
        then
            local matt, attp, acc, defp = xi.avatarsFavor.preSOADebuffs()
            pet:addMod(xi.mod.MATT, matt)
            pet:addMod(xi.mod.ATTP, attp)
            pet:addMod(xi.mod.ACC, acc)
            pet:addMod(xi.mod.DEFP, defp)
        end
    end
end

xi.avatarsFavor.applyAvatarsFavorAuraToPet = function(target, effect)
    local pet = target:getPet()
    if pet then
        local petId = pet:getPetID()
        if xi.avatarsFavor.shouldAvatarsFavorBeApplied(petId) then
            local power = xi.avatarsFavor.scalingFromParams({
                petId = petId,
                power = effect:getPower(),
            })
            local avatarEffect = avatarsFavorEffect[petId].effect

            --Useful debug message
            --printf('Power %d, Effect %d', effect:getPower(), power)

            pet:addStatusEffect(avatarEffect, { power = 6, duration = 15, origin = pet, tick = 3, subType = avatarEffect, subPower = power, tier = xi.auraTarget.ALLIES, flag = bit.bor(xi.effectFlag.NO_LOSS_MESSAGE, xi.effectFlag.AURA) })
        end
    end
end

xi.avatarsFavor.removeAvatarsFavorAuraFromPet = function(target)
    local pet = target:getPet()
    if pet then
        local petId = pet:getPetID()
        if xi.avatarsFavor.shouldAvatarsFavorBeApplied(petId) then
            if pet:hasStatusEffect(avatarsFavorEffect[petId].effect) then
                pet:delStatusEffect(avatarsFavorEffect[petId].effect)
            end

            removeAvatarsFavorDebuffsFromPet(target)
        end
    end
end

xi.avatarsFavor.applyAvatarsFavorDebuffsToPet = function(target)
    local pet = target:getPet()
    if pet then
        local petId = pet:getPetID()
        if  -- Different pet states for in and out of retail / eras
            xi.avatarsFavor.shouldAvatarsFavorBeApplied(petId) and
            xi.settings.main.ENABLE_SOA == 0
        then
            local matt, attp, acc, defp = xi.avatarsFavor.preSOADebuffs()
            pet:delMod(xi.mod.MATT, matt) -- Other than MATT most of these values are myth and guesses from multiple sources
            pet:delMod(xi.mod.ATTP, attp)
            pet:delMod(xi.mod.ACC, acc)
            pet:delMod(xi.mod.DEFP, defp)
        end
    end
end
