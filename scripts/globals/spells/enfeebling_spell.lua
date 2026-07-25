-----------------------------------
-- Enfeebling Spell Utilities
-- Used for spells that deal negative status effects upon targets.
--
-- Dual-wired pure inject forms (slice 6727 / 0872 / 6089):
--   elementalDebuffPotencyFromParams, calculatePotencyFromParams,
--   calculateDurationFromParams, immunobreakEligibleFromParams,
--   immunobreakChance, immunobreakSucceeds
-- Parity: internal/enfeebling
-----------------------------------
require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/jobpoints')
require('scripts/globals/magicburst')
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.enfeebling = xi.spells.enfeebling or {}
-----------------------------------
-- Pins matching internal/enfeebling.
xi.spells.enfeebling.skillEnfeeblingMagic = 35 -- xi.skill.ENFEEBLING_MAGIC
xi.spells.enfeebling.saboteurNMMult       = 1.3
xi.spells.enfeebling.saboteurNormalMult   = 2.0
xi.spells.enfeebling.immunobreakMinBaseRank  = 6
xi.spells.enfeebling.immunobreakMinFinalRank = 4
xi.spells.enfeebling.immunobreakBaseChanceFlat = 20
-----------------------------------
local column =
{
    EFFECT_ID      = 1,
    EFFECT_TIER    = 2,
    STAT_USED      = 3,
    BASE_POTENCY   = 4,
    BASE_TICK      = 5,
    BASE_DURATION  = 6,
    MESSAGE_OFFSET = 7,
    SABOTEUR       = 8,
    BONUS_MACC     = 9,
}

local pTable =
{
    -- Black Magic
    [xi.magic.spell.BIND          ] = { xi.effect.BIND,               1, xi.mod.INT,    0,   0,  60, 0, false,   0 },
    [xi.magic.spell.BINDGA        ] = { xi.effect.BIND,               1, xi.mod.INT,    0,   0,  60, 0, false,   0 },
    [xi.magic.spell.BLIND         ] = { xi.effect.BLINDNESS,          1, xi.mod.INT,    0,   0, 180, 0, true,    0 },
    [xi.magic.spell.BLIND_II      ] = { xi.effect.BLINDNESS,          3, xi.mod.INT,    0,   0, 180, 0, true,    0 },
    [xi.magic.spell.BLINDGA       ] = { xi.effect.BLINDNESS,          2, xi.mod.INT,    0,   0, 180, 0, true,    0 },
    [xi.magic.spell.BREAK         ] = { xi.effect.PETRIFICATION,      1, xi.mod.INT,    1,   0,  30, 0, false,   0 },
    [xi.magic.spell.BREAKGA       ] = { xi.effect.PETRIFICATION,      2, xi.mod.INT,    1,   0,  30, 0, false,   0 },
    [xi.magic.spell.BURN          ] = { xi.effect.BURN,               1, xi.mod.INT,    0,   3,  90, 1, true,    0 },
    [xi.magic.spell.CHOKE         ] = { xi.effect.CHOKE,              1, xi.mod.INT,    0,   3,  90, 1, true,    0 },
    [xi.magic.spell.CURSE         ] = { xi.effect.CURSE_I,            1, xi.mod.INT,   50,   0, 300, 0, false,   0 },
    [xi.magic.spell.DISPEL        ] = { xi.effect.NONE,               1, xi.mod.INT,    0,   0,   0, 0, false, 175 },
    [xi.magic.spell.DISPELGA      ] = { xi.effect.NONE,               1, xi.mod.INT,    0,   0,   0, 0, false,   0 },
    [xi.magic.spell.DISTRACT      ] = { xi.effect.EVASION_DOWN,       1, xi.mod.MND,    0,   0, 120, 0, true,  150 },
    [xi.magic.spell.DISTRACT_II   ] = { xi.effect.EVASION_DOWN,       2, xi.mod.MND,    0,   0, 120, 0, true,  150 },
    [xi.magic.spell.DISTRACT_III  ] = { xi.effect.EVASION_DOWN,       3, xi.mod.MND,    0,   0, 120, 0, true,  150 },
    [xi.magic.spell.DROWN         ] = { xi.effect.DROWN,              1, xi.mod.INT,    0,   3,  90, 1, true,    0 },
    [xi.magic.spell.FRAZZLE       ] = { xi.effect.MAGIC_EVASION_DOWN, 1, xi.mod.MND,    0,   0, 120, 0, true,  150 },
    [xi.magic.spell.FRAZZLE_II    ] = { xi.effect.MAGIC_EVASION_DOWN, 2, xi.mod.MND,    0,   0, 120, 0, true,  150 },
    [xi.magic.spell.FRAZZLE_III   ] = { xi.effect.MAGIC_EVASION_DOWN, 3, xi.mod.MND,    0,   0, 120, 0, true,  150 },
    [xi.magic.spell.FROST         ] = { xi.effect.FROST,              1, xi.mod.INT,    0,   3,  90, 1, true,    0 },
    [xi.magic.spell.GRAVITY       ] = { xi.effect.WEIGHT,             1, xi.mod.INT,   26,   0, 120, 0, true,    0 },
    [xi.magic.spell.GRAVITY_II    ] = { xi.effect.WEIGHT,             2, xi.mod.INT,   32,   0, 180, 0, true,    0 },
    [xi.magic.spell.GRAVIGA       ] = { xi.effect.WEIGHT,             1, xi.mod.INT,   50,   0, 120, 0, true,    0 },
    [xi.magic.spell.POISON        ] = { xi.effect.POISON,             1, xi.mod.INT,    0,   3,  90, 0, true,    0 },
    [xi.magic.spell.POISON_II     ] = { xi.effect.POISON,             2, xi.mod.INT,    0,   3, 120, 0, true,   30 },
    [xi.magic.spell.POISON_III    ] = { xi.effect.POISON,             3, xi.mod.INT,    0,   3, 150, 0, true,    0 },
    [xi.magic.spell.POISONGA      ] = { xi.effect.POISON,             1, xi.mod.INT,    0,   3,  90, 0, true,    0 },
    [xi.magic.spell.POISONGA_II   ] = { xi.effect.POISON,             1, xi.mod.INT,    0,   3, 120, 0, true,    0 },
    [xi.magic.spell.POISONGA_III  ] = { xi.effect.POISON,             1, xi.mod.INT,    0,   3, 150, 0, true,    0 },
    [xi.magic.spell.RASP          ] = { xi.effect.RASP,               1, xi.mod.INT,    0,   3,  90, 1, true,    0 },
    [xi.magic.spell.SHOCK         ] = { xi.effect.SHOCK,              1, xi.mod.INT,    0,   3,  90, 1, true,    0 },
    [xi.magic.spell.SLEEP         ] = { xi.effect.SLEEP_I,            1, xi.mod.INT,    1,   0,  60, 0, false,   0 },
    [xi.magic.spell.SLEEP_II      ] = { xi.effect.SLEEP_I,            2, xi.mod.INT,    2,   0,  90, 0, false,   0 },
    [xi.magic.spell.SLEEPGA       ] = { xi.effect.SLEEP_I,            1, xi.mod.INT,    1,   0,  60, 0, false,   0 },
    [xi.magic.spell.SLEEPGA_II    ] = { xi.effect.SLEEP_I,            2, xi.mod.INT,    2,   0,  90, 0, false,   0 },
    [xi.magic.spell.STUN          ] = { xi.effect.STUN,               1, xi.mod.INT,    1,   0,   5, 0, false, 200 },
    [xi.magic.spell.VIRUS         ] = { xi.effect.PLAGUE,             1, xi.mod.INT,    5,   3,  60, 0, false,   0 },

    -- Black magic Helixes
    [xi.magic.spell.GEOHELIX      ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.GEOHELIX_II   ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.HYDROHELIX    ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.HYDROHELIX_II ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.ANEMOHELIX    ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.ANEMOHELIX_II ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.PYROHELIX     ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.PYROHELIX_II  ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.CRYOHELIX     ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.CRYOHELIX_II  ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.IONOHELIX     ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.IONOHELIX_II  ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.NOCTOHELIX    ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.NOCTOHELIX_II ] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.LUMINOHELIX   ] = { xi.effect.HELIX,              1, xi.mod.INT,    0,  10,  30, 0, false,   0 },
    [xi.magic.spell.LUMINOHELIX_II] = { xi.effect.HELIX,              2, xi.mod.INT,    0,  10,  30, 0, false,   0 },

    -- White Magic
    [xi.magic.spell.ADDLE         ] = { xi.effect.ADDLE,              1, xi.mod.MND,   20,   0, 180, 0, true,   20 },
    [xi.magic.spell.ADDLE_II      ] = { xi.effect.ADDLE,              2, xi.mod.MND,   50,   0, 180, 0, true,   20 },
    [xi.magic.spell.FLASH         ] = { xi.effect.FLASH,              1, xi.mod.MND,    0,   0,  12, 0, true,  512 },
    [xi.magic.spell.INUNDATION    ] = { xi.effect.INUNDATION,         1, xi.mod.MND,    1,   0, 300, 0, false,   0 },
    [xi.magic.spell.PARALYZE      ] = { xi.effect.PARALYSIS,          1, xi.mod.MND,    0,   0, 120, 0, true,  -10 },
    [xi.magic.spell.PARALYZE_II   ] = { xi.effect.PARALYSIS,          3, xi.mod.MND,    0,   0, 120, 0, true,    0 },
    [xi.magic.spell.PARALYGA      ] = { xi.effect.PARALYSIS,          2, xi.mod.MND,    0,   0, 120, 0, true,    0 },
    [xi.magic.spell.REPOSE        ] = { xi.effect.SLEEP_I,            2, xi.mod.MND,    2,   0,  90, 1, false,   0 },
    [xi.magic.spell.SILENCE       ] = { xi.effect.SILENCE,            1, xi.mod.MND,    1,   0, 120, 0, false,   0 },
    [xi.magic.spell.SILENCEGA     ] = { xi.effect.SILENCE,            2, xi.mod.MND,    1,   0, 120, 0, false,   0 },
    [xi.magic.spell.SLOW          ] = { xi.effect.SLOW,               3, xi.mod.MND,    0,   0, 180, 0, true,   10 },
    [xi.magic.spell.SLOW_II       ] = { xi.effect.SLOW,               7, xi.mod.MND,    0,   0, 180, 0, true,   10 },
    [xi.magic.spell.SLOWGA        ] = { xi.effect.SLOW,               8, xi.mod.MND,    0,   0, 180, 0, true,    0 },

    -- Ninjutsu
    [xi.magic.spell.AISHA_ICHI    ] = { xi.effect.ATTACK_DOWN,        1, xi.mod.INT,   15,   0, 120, 1, false,   0 },
    [xi.magic.spell.DOKUMORI_ICHI ] = { xi.effect.POISON,             1, xi.mod.INT,    3,   3,  60, 0, false,   0 },
    [xi.magic.spell.DOKUMORI_NI   ] = { xi.effect.POISON,             2, xi.mod.INT,   10,   3, 120, 0, false,   0 },
    [xi.magic.spell.DOKUMORI_SAN  ] = { xi.effect.POISON,             3, xi.mod.INT,   20,   3, 360, 0, false,   0 },
    [xi.magic.spell.HOJO_ICHI     ] = { xi.effect.SLOW,               3, xi.mod.INT, 1465,   0, 180, 0, false,   0 },
    [xi.magic.spell.HOJO_NI       ] = { xi.effect.SLOW,               4, xi.mod.INT, 1953,   0, 300, 0, false,   0 },
    [xi.magic.spell.HOJO_SAN      ] = { xi.effect.SLOW,               7, xi.mod.INT, 2930,   0, 420, 0, false,   0 },
    [xi.magic.spell.JUBAKU_ICHI   ] = { xi.effect.PARALYSIS,          1, xi.mod.INT,   20,   0, 180, 1, false,   0 },
    [xi.magic.spell.JUBAKU_NI     ] = { xi.effect.PARALYSIS,          2, xi.mod.INT,   30,   0, 300, 1, false,   0 },
    [xi.magic.spell.JUBAKU_SAN    ] = { xi.effect.PARALYSIS,          3, xi.mod.INT,   35,   0, 420, 1, false,   0 },
    [xi.magic.spell.KURAYAMI_ICHI ] = { xi.effect.BLINDNESS,          1, xi.mod.INT,   20,   0, 180, 0, false,   0 },
    [xi.magic.spell.KURAYAMI_NI   ] = { xi.effect.BLINDNESS,          2, xi.mod.INT,   30,   0, 300, 0, false,   0 },
    [xi.magic.spell.KURAYAMI_SAN  ] = { xi.effect.BLINDNESS,          3, xi.mod.INT,   40,   0, 420, 0, false,   0 },
    [xi.magic.spell.YURIN_ICHI    ] = { xi.effect.INHIBIT_TP,         1, xi.mod.INT,   10,   0, 180, 1, false,   0 },
}

-- Pure elemental debuff potency (internal/enfeebling.ElementalDebuffPotency).
-- params: casterStat, merit, mod
xi.spells.enfeebling.elementalDebuffPotencyFromParams = function(params)
    params = params or {}
    local potency = 1
    local casterStat = params.casterStat or 0

    if casterStat > 150 then
        potency = potency + 4
    elseif casterStat > 100 then
        potency = potency + 3
    elseif casterStat > 70 then
        potency = potency + 2
    elseif casterStat > 40 then
        potency = potency + 1
    end

    return potency + (params.merit or 0) + (params.mod or 0) / 2
end

-- Host wrapper for elemental ladder (entity inject).
local function getElementalDebuffPotency(caster, statUsed)
    return xi.spells.enfeebling.elementalDebuffPotencyFromParams({
        casterStat = caster:getStat(statUsed),
        merit      = caster:getMerit(xi.merit.ELEMENTAL_DEBUFF_EFFECT),
        mod        = caster:getMod(xi.mod.ELEMENTAL_DEBUFF_EFFECT),
    })
end

-- Pure immunobreak gates (internal/enfeebling.ImmunobreakEligible).
xi.spells.enfeebling.immunobreakEligibleFromParams = function(params)
    params = params or {}
    if not params.enabled or not params.casterIsPC or not params.targetIsMob then
        return false
    end

    if (params.skillType or 0) ~= xi.spells.enfeebling.skillEnfeeblingMagic then
        return false
    end

    if (params.immunobreakModId or 0) == 0 then
        return false
    end

    if (params.baseResistanceRank or 0) < xi.spells.enfeebling.immunobreakMinBaseRank then
        return false
    end

    local finalRank = (params.baseResistanceRank or 0) - (params.immunobreakValue or 0)
    return finalRank > xi.spells.enfeebling.immunobreakMinFinalRank
end

-- Pure chance: merit + 20/(value+1)
xi.spells.enfeebling.immunobreakChance = function(merit, immunobreakValue)
    return (merit or 0) + xi.spells.enfeebling.immunobreakBaseChanceFlat / ((immunobreakValue or 0) + 1)
end

-- Pure roll: succeed when roll (1..100) <= chance
xi.spells.enfeebling.immunobreakSucceeds = function(chance, roll1to100)
    return (roll1to100 or 0) <= (chance or 0)
end

local function executeImmunobreak(caster, target, spell, effectId)
    -- Fetch resistance rank modifier (Either effect-specific or elemental)
    local immunobreakModId = xi.data.statusEffect.getAssociatedImmunobreakModifier(effectId)
    local resistanceRankModId = xi.data.statusEffect.getAssociatedResistanceRankModifier(effectId, spell:getElement())
    if resistanceRankModId == 0 then -- If it's an effect and this is 0, try with element.
        resistanceRankModId = xi.data.element.getElementalResistanceRankModifier(spell:getElement())
    end

    local baseResistanceRank = target:getMod(resistanceRankModId)
    local immunobreakValue   = target:getMod(immunobreakModId)

    if not xi.spells.enfeebling.immunobreakEligibleFromParams({
        enabled             = xi.settings.main.ENABLE_IMMUNOBREAK and true or false,
        casterIsPC          = caster:isPC(),
        targetIsMob         = target:isMob(),
        skillType           = spell:getSkillType(),
        immunobreakModId    = immunobreakModId,
        baseResistanceRank  = baseResistanceRank,
        immunobreakValue    = immunobreakValue,
    }) then
        return
    end

    local chance = xi.spells.enfeebling.immunobreakChance(
        caster:getMerit(xi.merit.IMMUNOBREAK_CHANCE),
        immunobreakValue
    )
    if not xi.spells.enfeebling.immunobreakSucceeds(chance, math.random(1, 100)) then
        return
    end

    -- Apply immunobreak effect (lower resistance rank) and apply special message.
    target:setMod(immunobreakModId, immunobreakValue + 1) -- TODO: Add equipment modifier (x2) here (Chironic Hose).
    spell:setModifier(xi.msg.actionModifier.IMMUNOBREAK)
end

-- Pure calculatePotency (internal/enfeebling.Potency).
-- params: spellId, spellEffect, basePotency, skillType, skillLevel,
--         casterStat, targetStat, targetMND, elementalDebuffMerit, elementalDebuffMod,
--         applySaboteur, hasSaboteur, targetIsNM, enhancesSaboteur, enfMagPotency
xi.spells.enfeebling.calculatePotencyFromParams = function(params)
    params = params or {}
    local potency    = params.basePotency or 0
    local spellId    = params.spellId or 0
    local spellEffect = params.spellEffect or 0
    local skillLevel = params.skillLevel or 0
    local casterStat = params.casterStat or 0
    local targetStat = params.targetStat or 0
    local targetMND  = params.targetMND or 0
    local statDiff   = casterStat - targetStat

    if spellEffect == xi.effect.ADDLE then
        potency = potency + utils.clamp(math.floor(statDiff / 5), 0, 20)
    elseif spellEffect == xi.effect.BLINDNESS then
        local blindDiff = casterStat - targetMND
        if spellId == xi.magic.spell.BLIND_II then
            potency = utils.clamp(blindDiff * 0.375 + 49, 19, 94)
        else
            potency = utils.clamp(blindDiff * 0.225 + 23, 5, 50)
        end
    elseif spellEffect == xi.effect.EVASION_DOWN then
        if spellId == xi.magic.spell.DISTRACT then
            potency = utils.clamp(skillLevel / 5, 0, 25) + utils.clamp(statDiff / 5, 0, 10)
        elseif spellId == xi.magic.spell.DISTRACT_II then
            potency = utils.clamp(skillLevel * 4 / 35, 0, 40) + utils.clamp(statDiff / 5, 0, 10)
        else
            potency = utils.clamp(skillLevel / 5, 0, 120) + utils.clamp(statDiff / 5, 0, 10)
        end
    elseif spellEffect == xi.effect.MAGIC_EVASION_DOWN then
        if spellId == xi.magic.spell.FRAZZLE then
            potency = utils.clamp(skillLevel / 5, 0, 25) + utils.clamp(statDiff / 5, 0, 10)
        elseif spellId == xi.magic.spell.FRAZZLE_II then
            potency = utils.clamp(skillLevel * 4 / 35, 0, 40) + utils.clamp(statDiff / 5, 0, 10)
        else
            potency = utils.clamp(skillLevel / 5, 0, 120) + utils.clamp(statDiff / 5, 0, 10)
        end
    elseif spellEffect == xi.effect.PARALYSIS then
        if spellId == xi.magic.spell.PARALYZE_II then
            potency = utils.clamp(statDiff / 4 + 24, 14, 34)
        else
            potency = utils.clamp(statDiff / 4 + 15, 5, 25)
        end
    elseif spellEffect == xi.effect.POISON then
        if
            spellId == xi.magic.spell.POISON or
            spellId == xi.magic.spell.POISONGA
        then
            potency = math.max(skillLevel / 25, 1)
            if skillLevel > 400 then
                potency = math.min((skillLevel - 225) / 5, 55)
            end
        elseif
            spellId == xi.magic.spell.POISON_II or
            spellId == xi.magic.spell.POISONGA_II
        then
            potency = math.max(skillLevel / 20, 4)
            if skillLevel > 400 then
                potency = skillLevel * 49 / 183 - 55
            end
        else
            potency = skillLevel / 10 + 1
        end
    elseif spellEffect == xi.effect.SLOW then
        if spellId == xi.magic.spell.SLOW_II then
            potency = utils.clamp(statDiff * 226 / 15 + 2780, 1650, 3910)
        else
            potency = utils.clamp(statDiff * 73 / 5 + 1825, 730, 2920)
        end
    elseif
        spellEffect == xi.effect.BURN or
        spellEffect == xi.effect.CHOKE or
        spellEffect == xi.effect.DROWN or
        spellEffect == xi.effect.FROST or
        spellEffect == xi.effect.RASP or
        spellEffect == xi.effect.SHOCK
    then
        potency = xi.spells.enfeebling.elementalDebuffPotencyFromParams({
            casterStat = casterStat,
            merit      = params.elementalDebuffMerit or 0,
            mod        = params.elementalDebuffMod or 0,
        })
    end

    potency = math.floor(potency)

    if
        params.applySaboteur and
        params.hasSaboteur and
        (params.skillType or 0) == xi.spells.enfeebling.skillEnfeeblingMagic
    then
        local enhance = params.enhancesSaboteur or 0
        if params.targetIsNM then
            potency = math.floor(potency * (xi.spells.enfeebling.saboteurNMMult + enhance))
        else
            potency = math.floor(potency * (xi.spells.enfeebling.saboteurNormalMult + enhance))
        end
    end

    potency = math.floor(potency * (1 + (params.enfMagPotency or 0) / 100))

    return potency
end

-- Calculate potency (host → pure).
xi.spells.enfeebling.calculatePotency = function(caster, target, spellId, spellEffect, skillType, statUsed)
    local basePotency = pTable[spellId][column.BASE_POTENCY]
    local applySaboteur = pTable[spellId][column.SABOTEUR]

    return xi.spells.enfeebling.calculatePotencyFromParams({
        spellId              = spellId,
        spellEffect          = spellEffect,
        basePotency          = basePotency,
        skillType            = skillType,
        skillLevel           = caster:getSkillLevel(skillType),
        casterStat           = caster:getStat(statUsed),
        targetStat           = target:getStat(statUsed),
        targetMND            = target:getStat(xi.mod.MND),
        elementalDebuffMerit = caster:getMerit(xi.merit.ELEMENTAL_DEBUFF_EFFECT),
        elementalDebuffMod   = caster:getMod(xi.mod.ELEMENTAL_DEBUFF_EFFECT),
        applySaboteur        = applySaboteur,
        hasSaboteur          = caster:hasStatusEffect(xi.effect.SABOTEUR),
        targetIsNM           = target:isNM(),
        enhancesSaboteur     = caster:getMod(xi.mod.ENHANCES_SABOTEUR),
        enfMagPotency        = caster:getMod(xi.mod.ENF_MAG_POTENCY),
    })
end

-- Pure calculateDuration before resist (internal/enfeebling.Duration).
-- params: spellEffect, baseDuration, skillType, bindDuration,
--         elementalDebuffDurationMerit, casterMainLvl, hasDarkArts, darkArtsJP,
--         helixDurationMod, hasSaboteur, targetIsNM, isRDM,
--         enfeeblingDurationMerit, enfeebleDurationJP, hasStymie, stymieJP,
--         enfMagDuration
xi.spells.enfeebling.calculateDurationFromParams = function(params)
    params = params or {}
    local duration    = params.baseDuration or 0
    local spellEffect = params.spellEffect or 0

    if spellEffect == xi.effect.BIND then
        if (params.bindDuration or 0) > 0 then
            duration = params.bindDuration
        end
    end

    if
        spellEffect == xi.effect.BURN or
        spellEffect == xi.effect.CHOKE or
        spellEffect == xi.effect.DROWN or
        spellEffect == xi.effect.FROST or
        spellEffect == xi.effect.RASP or
        spellEffect == xi.effect.SHOCK
    then
        duration = duration + (params.elementalDebuffDurationMerit or 0)
    elseif spellEffect == xi.effect.HELIX then
        local casterLevel = params.casterMainLvl or 0
        if casterLevel >= 60 then
            duration = duration + 60
        elseif casterLevel >= 40 then
            duration = duration + 30
        end

        if params.hasDarkArts then
            duration = duration + 3 * (params.darkArtsJP or 0)
        end

        duration = duration + (params.helixDurationMod or 0)
    end

    if (params.skillType or 0) == xi.spells.enfeebling.skillEnfeeblingMagic then
        if params.hasSaboteur then
            if params.targetIsNM then
                duration = duration * 1.25
            else
                duration = duration * 2
            end
        end

        if params.isRDM then
            duration = duration + (params.enfeeblingDurationMerit or 0)
            duration = duration + (params.enfeebleDurationJP or 0)
            if params.hasStymie then
                duration = duration + (params.stymieJP or 0)
            end
        end

        duration = math.floor(duration * (1 + (params.enfMagDuration or 0) / 100))
    end

    return math.floor(duration)
end

-- Calculate duration before resist (host → pure).
xi.spells.enfeebling.calculateDuration = function(caster, target, spellId, spellEffect, skillType)
    local baseDuration = pTable[spellId][column.BASE_DURATION]
    local bindDuration = 0
    if spellEffect == xi.effect.BIND then
        bindDuration = math.random(13, 60)
    end

    return xi.spells.enfeebling.calculateDurationFromParams({
        spellEffect                  = spellEffect,
        baseDuration                 = baseDuration,
        skillType                    = skillType,
        bindDuration                 = bindDuration,
        elementalDebuffDurationMerit = caster:getMerit(xi.merit.ELEMENTAL_DEBUFF_DURATION),
        casterMainLvl                = caster:getMainLvl(),
        hasDarkArts                  = caster:hasStatusEffect(xi.effect.DARK_ARTS),
        darkArtsJP                   = caster:getJobPointLevel(xi.jp.DARK_ARTS_EFFECT),
        helixDurationMod             = caster:getMod(xi.mod.HELIX_DURATION),
        hasSaboteur                  = caster:hasStatusEffect(xi.effect.SABOTEUR),
        targetIsNM                   = target:isNM(),
        isRDM                        = caster:getMainJob() == xi.job.RDM,
        enfeeblingDurationMerit      = caster:getMerit(xi.merit.ENFEEBLING_MAGIC_DURATION),
        enfeebleDurationJP           = caster:getJobPointLevel(xi.jp.ENFEEBLE_DURATION),
        hasStymie                    = caster:hasStatusEffect(xi.effect.STYMIE),
        stymieJP                     = caster:getJobPointLevel(xi.jp.STYMIE_EFFECT),
        enfMagDuration               = caster:getMod(xi.mod.ENF_MAG_DURATION),
    })
end

-- Main function, called by spell scripts
xi.spells.enfeebling.useEnfeeblingSpell = function(caster, target, spell)
    local spellId      = spell:getID()
    local spellElement = spell:getElement()
    local skillType    = spell:getSkillType()
    local spellEffect  = pTable[spellId][column.EFFECT_ID]
    local tier         = pTable[spellId][column.EFFECT_TIER] or 0

    ------------------------------
    -- STEP 1: Check spell nullification.
    ------------------------------
    if xi.data.statusEffect.isTargetImmune(target, spellEffect, spellElement) then
        spell:setMsg(xi.msg.basic.MAGIC_COMPLETE_RESIST)
        return spellEffect
    end

    -- Check trait nullification trigger.
    if xi.data.statusEffect.isTargetResistant(caster, target, spellEffect) then
        spell:setModifier(xi.msg.actionModifier.RESIST)
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)
        return spellEffect
    end

    -- Target already has an status effect that nullifies current.
    if xi.data.statusEffect.isEffectNullified(target, spellEffect, tier) then
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
        return spellEffect
    end

    ------------------------------
    -- STEP 2: Calculate resist tiers.
    ------------------------------
    local spellGroup = spell:getSpellGroup()
    local statUsed   = pTable[spellId][column.STAT_USED]
    local message    = pTable[spellId][column.MESSAGE_OFFSET]
    local bonusMacc  = pTable[spellId][column.BONUS_MACC]
    local resistRate = xi.combat.magicHitRate.calculateResistRate(caster, target, spellGroup, skillType, 0, spellElement, statUsed, spellEffect, bonusMacc)

    if spellEffect ~= xi.effect.NONE then
        -- Stymie
        if
            skillType == xi.skill.ENFEEBLING_MAGIC and
            caster:hasStatusEffect(xi.effect.STYMIE)
        then
            resistRate = 1

        -- Fealty
        elseif target:hasStatusEffect(xi.effect.FEALTY) then
            resistRate = 0
        end
    end

    ------------------------------
    -- STEP 3: Check if spell resists.
    ------------------------------
    if not xi.data.statusEffect.isResistRateSuccessfull(spellEffect, resistRate, 0) then
        executeImmunobreak(caster, target, spell, spellEffect)
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)

        return spellEffect
    end

    ------------------------------
    -- STEP 4: Calculate Duration, Potency, Tick and Sub-Potency (additional effects)
    ------------------------------
    local potency    = xi.spells.enfeebling.calculatePotency(caster, target, spellId, spellEffect, skillType, statUsed)
    local subpotency = 0
    local duration   = math.floor(xi.spells.enfeebling.calculateDuration(caster, target, spellId, spellEffect, skillType) * resistRate)
    local tick       = pTable[spellId][column.BASE_TICK]

    ------------------------------
    -- STEP 5: Exceptions.
    ------------------------------
    -- Bind: Dependant on target speed.
    -- Bind: Duration floor of 5 seconds.
    if spellEffect == xi.effect.BIND then
        potency = target:getSpeed()
        duration = utils.clamp(duration, 5, 60)

    -- TODO: This is unnecesary, but, for now, we will comply with core.
    elseif spellEffect == xi.effect.SLEEP_I then
        subpotency = spellElement

    -- Addle: Sub-effect -> Slows casting time.
    elseif spellEffect == xi.effect.ADDLE then
        subpotency = 20 + utils.clamp(math.floor((caster:getStat(statUsed) - target:getStat(statUsed)) / 5), 0, 20)

    -- Break: Player petrification sucks.
    elseif spellEffect == xi.effect.PETRIFICATION then
        if caster:isPC() then
            subpotency = 1
        end

    -- Dispel: It's special in that it has no real effect.
    elseif spellEffect == xi.effect.NONE then
        spellEffect = target:dispelStatusEffect()

        if spellEffect == xi.effect.NONE then
            spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
        else
            spell:setMsg(xi.msg.basic.MAGIC_ERASE)
        end

        return spellEffect
    end

    ------------------------------
    -- STEP 6: Final Operations.
    ------------------------------
    if target:addStatusEffect(spellEffect, { power = potency, duration = duration, origin = caster, tick = tick, subPower = subpotency, tier = tier }) then
        -- Delete Stymie effect
        if
            skillType == xi.skill.ENFEEBLING_MAGIC and
            caster:hasStatusEffect(xi.effect.STYMIE)
        then
            caster:delStatusEffect(xi.effect.STYMIE)
        end

        -- Add "Magic Burst!" message
        local _, skillchainCount = xi.magicburst.formMagicBurst(target, spellElement) -- External function. Not present in magic.lua.

        if skillchainCount > 0 then
            spell:setMsg(xi.msg.basic.MAGIC_BURST_ENFEEB_IS - message * 3)
            caster:triggerRoeEvent(xi.roeTrigger.MAGIC_BURST)
        else
            spell:setMsg(xi.msg.basic.MAGIC_ENFEEB_IS + message)
        end
    else
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
    end

    return spellEffect
end
