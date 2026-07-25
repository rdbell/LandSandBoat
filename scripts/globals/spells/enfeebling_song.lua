-----------------------------------
-- Enfeebling Song Utilities
-- Used for songs that deal negative status effects upon targets.
--
-- Dual-wired pure inject forms (slice 6728 / 0873):
--   calculateSongPowerFromParams, calculateSongDurationFromParams,
--   clampSongPower, finaleBonusMacc, requiemTickFor,
--   successMessageFromParams, virelaiMessage
-- Parity: internal/enfeeblingsong
-----------------------------------
require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/jobpoints')
require('scripts/globals/magicburst')
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.enfeebling = xi.spells.enfeebling or {}
-----------------------------------
local column =
{
    SONG_EFFECT     = 1,
    SONG_TIER       = 2,
    SONG_POWER_BASE = 3,
    SONG_POWER_CAP  = 4,
    SONG_DURATION   = 5,
    SONG_MODIFIER   = 6,
}

local pTable =
{
    -- [Spell ID                         ] = { Effect,           Tier,  Base,  Cap, Dur, Modifier               },
    -- Requiem: https://www.bg-wiki.com/ffxi/Category:Requiem
    [xi.magic.spell.FOE_REQUIEM          ] = { xi.effect.REQUIEM,  1,     1,   300,  64, xi.mod.REQUIEM_EFFECT  },
    [xi.magic.spell.FOE_REQUIEM_II       ] = { xi.effect.REQUIEM,  2,     2,   300,  80, xi.mod.REQUIEM_EFFECT  },
    [xi.magic.spell.FOE_REQUIEM_III      ] = { xi.effect.REQUIEM,  3,     3,   300,  96, xi.mod.REQUIEM_EFFECT  },
    [xi.magic.spell.FOE_REQUIEM_IV       ] = { xi.effect.REQUIEM,  4,     4,   300, 112, xi.mod.REQUIEM_EFFECT  },
    [xi.magic.spell.FOE_REQUIEM_V        ] = { xi.effect.REQUIEM,  5,     5,   300, 128, xi.mod.REQUIEM_EFFECT  },
    [xi.magic.spell.FOE_REQUIEM_VI       ] = { xi.effect.REQUIEM,  6,     6,   300, 144, xi.mod.REQUIEM_EFFECT  },
    [xi.magic.spell.FOE_REQUIEM_VII      ] = { xi.effect.REQUIEM,  7,     8,   300, 160, xi.mod.REQUIEM_EFFECT  },
    -- Lullaby: https://www.bg-wiki.com/ffxi/Category:Lullaby
    [xi.magic.spell.FOE_LULLABY          ] = { xi.effect.SLEEP_I,  1,     1,     1,  30, xi.mod.LULLABY_EFFECT  },
    [xi.magic.spell.FOE_LULLABY_II       ] = { xi.effect.SLEEP_I,  1,     1,     1,  60, xi.mod.LULLABY_EFFECT  },
    [xi.magic.spell.HORDE_LULLABY        ] = { xi.effect.SLEEP_I,  1,     1,     1,  30, xi.mod.LULLABY_EFFECT  },
    [xi.magic.spell.HORDE_LULLABY_II     ] = { xi.effect.SLEEP_I,  1,     1,     1,  60, xi.mod.LULLABY_EFFECT  },
    -- Finale: https://www.bg-wiki.com/ffxi/Category:Finale
    [xi.magic.spell.MAGIC_FINALE         ] = { xi.effect.NONE,     1,     1,     1,   0, xi.mod.FINALE_EFFECT   },
    -- Elegy: https://www.bg-wiki.com/ffxi/Category:Elegy
    [xi.magic.spell.BATTLEFIELD_ELEGY    ] = { xi.effect.ELEGY,    1,  2500,  5000, 120, xi.mod.ELEGY_EFFECT    },
    [xi.magic.spell.CARNAGE_ELEGY        ] = { xi.effect.ELEGY,    1,  5000,  5000, 180, xi.mod.ELEGY_EFFECT    },
    [xi.magic.spell.MASSACRE_ELEGY       ] = { xi.effect.ELEGY,    1, 10000, 10000, 180, xi.mod.ELEGY_EFFECT    },
    -- Threnody: https://www.bg-wiki.com/ffxi/Category:Threnody
    [xi.magic.spell.FIRE_THRENODY        ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.ICE_THRENODY         ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.WIND_THRENODY        ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.EARTH_THRENODY       ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.LIGHTNING_THRENODY   ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.WATER_THRENODY       ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.LIGHT_THRENODY       ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.DARK_THRENODY        ] = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.FIRE_THRENODY_II     ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.ICE_THRENODY_II      ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.WIND_THRENODY_II     ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.EARTH_THRENODY_II    ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.LIGHTNING_THRENODY_II] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.WATER_THRENODY_II    ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.LIGHT_THRENODY_II    ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    [xi.magic.spell.DARK_THRENODY_II     ] = { xi.effect.THRENODY, 2,   160,   205,  90, xi.mod.THRENODY_EFFECT },
    -- Virelai: https://www.bg-wiki.com/ffxi/Category:Virelai
    [xi.magic.spell.MAIDENS_VIRELAI      ] = { xi.effect.CHARM_I,  1,     0,     0,  30, xi.mod.VIRELAI_EFFECT  },
    -- Nocturne: https://www.bg-wiki.com/ffxi/Category:Nocturne
    [xi.magic.spell.PINING_NOCTURNE      ] = { xi.effect.NOCTURNE, 1,    15,    25, 120, 0                      },
}

-----------------------------------
-- Pure song power / duration injects (slice 6728)
-- Parity: internal/enfeeblingsong
-----------------------------------

-- Pure calculateSongPower (internal/enfeeblingsong.SongPower).
-- params: spellEffect, basePower, gearBoost, requiemJP,
--         hasSoulVoice, hasMarcato, marcatoPower
xi.spells.enfeebling.calculateSongPowerFromParams = function(params)
    params = params or {}
    local power       = params.basePower or 0
    local gearBoost   = params.gearBoost or 0
    local spellEffect = params.spellEffect or 0

    if spellEffect == xi.effect.REQUIEM then
        power = power + utils.clamp(gearBoost - 1, 0, 20) + (params.requiemJP or 0) * 3
    elseif spellEffect == xi.effect.ELEGY then
        power = power + gearBoost * 6375 / 256 -- Simplified numbers of: 25.5 * 10000/1024
    elseif spellEffect == xi.effect.THRENODY then
        power = power + gearBoost * 5
    elseif spellEffect == xi.effect.NOCTURNE then
        power = power + gearBoost * 1.5
    end

    -- Soul Voice / Marcato for Elegy/Nocturne/Requiem/Threnody only.
    if
        spellEffect == xi.effect.ELEGY or
        spellEffect == xi.effect.NOCTURNE or
        spellEffect == xi.effect.REQUIEM or
        spellEffect == xi.effect.THRENODY
    then
        if params.hasSoulVoice then
            power = power * 2
        elseif params.hasMarcato then
            power = power * (1 + (params.marcatoPower or 0) / 100)
        end
    end

    return power
end

-- Pure calculateSongDuration (internal/enfeeblingsong.SongDuration).
-- params: spellEffect, baseDuration, gearBoost, songDurationBonus, lullabyJP,
--         hasClarionCall, clarionCallJP, hasTenuto, tenutoJP, hasTroubadour
xi.spells.enfeebling.calculateSongDurationFromParams = function(params)
    params = params or {}
    local duration    = params.baseDuration or 0
    local gearBoost   = params.gearBoost or 0
    local spellEffect = params.spellEffect or 0

    -- Virelai: skill/gear only; early return (no SONG_DURATION_BONUS / status tails).
    if spellEffect == xi.effect.CHARM_I then
        return math.floor(duration * (1 + gearBoost / 10))
    end

    duration = math.floor(duration * (1 + gearBoost / 10 + (params.songDurationBonus or 0) / 100))

    if spellEffect == xi.effect.SLEEP_I then
        duration = duration + (params.lullabyJP or 0)
    end

    if params.hasClarionCall then
        duration = duration + (params.clarionCallJP or 0) * 2
    end

    if params.hasTenuto then
        duration = duration + (params.tenutoJP or 0) * 2
    end

    if params.hasTroubadour then
        duration = math.floor(duration * 2)
    end

    return duration
end

-- Pure power clamp (internal/enfeeblingsong.ClampPower).
xi.spells.enfeebling.clampSongPower = function(power, cap)
    power = power or 0
    cap   = cap or 0
    if power < 0 then
        power = 0
    end

    if cap >= 0 and power > cap then
        power = cap
    end

    return math.floor(power)
end

-- Pure Finale bonus macc (internal/enfeeblingsong.FinaleBonusMacc).
xi.spells.enfeebling.finaleBonusMacc = function(gearBoost)
    return 175 + (gearBoost or 0) * 5
end

-- Pure Requiem tick (internal/enfeeblingsong.RequiemTickFor).
xi.spells.enfeebling.requiemTickFor = function(spellEffect)
    if spellEffect == xi.effect.REQUIEM then
        return 3
    end

    return 0
end

-- Pure success message (internal/enfeeblingsong.SuccessMessage).
-- MAGIC_BURST_ENFEEB=268, MAGIC_ENFEEB_IS=236, MAGIC_ENFEEB=237
xi.spells.enfeebling.successMessageFromParams = function(params)
    params = params or {}
    if (params.skillchainCount or 0) > 0 then
        return 268
    end

    if params.spellEffect == xi.effect.SLEEP_I then
        return 236
    end

    return 237
end

-- Pure Virelai message (internal/enfeeblingsong.VirelaiMessage).
xi.spells.enfeebling.virelaiMessage = function(casterIsPC)
    if casterIsPC then
        return 237 -- MAGIC_ENFEEB
    end

    return 236 -- MAGIC_ENFEEB_IS
end

-----------------------------------
-- Calculates song power (host → pure).
-----------------------------------
xi.spells.enfeebling.calculateSongPower = function(caster, spellEffect, basePower, gearBoost)
    local marcatoEffect = caster:getStatusEffect(xi.effect.MARCATO)
    local hasMarcato    = marcatoEffect ~= nil
    local marcatoPower  = hasMarcato and marcatoEffect:getPower() or 0

    return xi.spells.enfeebling.calculateSongPowerFromParams({
        spellEffect  = spellEffect,
        basePower    = basePower,
        gearBoost    = gearBoost,
        requiemJP    = caster:getJobPointLevel(xi.jp.REQUIEM_EFFECT),
        hasSoulVoice = caster:hasStatusEffect(xi.effect.SOUL_VOICE),
        hasMarcato   = hasMarcato,
        marcatoPower = marcatoPower,
    })
end

-----------------------------------
-- Calculates song duration (host → pure).
-----------------------------------
xi.spells.enfeebling.calculateSongDuration = function(caster, spellEffect, baseDuration, gearBoost)
    return xi.spells.enfeebling.calculateSongDurationFromParams({
        spellEffect       = spellEffect,
        baseDuration      = baseDuration,
        gearBoost         = gearBoost,
        songDurationBonus = caster:getMod(xi.mod.SONG_DURATION_BONUS),
        lullabyJP         = caster:getJobPointLevel(xi.jp.LULLABY_DURATION),
        hasClarionCall    = caster:hasStatusEffect(xi.effect.CLARION_CALL),
        clarionCallJP     = caster:getJobPointLevel(xi.jp.CLARION_CALL_EFFECT),
        hasTenuto         = caster:hasStatusEffect(xi.effect.TENUTO),
        tenutoJP          = caster:getJobPointLevel(xi.jp.TENUTO_EFFECT),
        hasTroubadour     = caster:hasStatusEffect(xi.effect.TROUBADOUR),
    })
end

-----------------------------------
-- Casts an enfeebling song.
-----------------------------------
xi.spells.enfeebling.useEnfeeblingSong = function(caster, target, spell)
    local spellId      = spell:getID()
    local spellElement = spell:getElement()
    local spellEffect  = pTable[spellId][column.SONG_EFFECT]
    local spellTier    = pTable[spellId][column.SONG_TIER]

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
    if xi.data.statusEffect.isEffectNullified(target, spellEffect, spellTier) then
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
        return spellEffect
    end

    ------------------------------
    -- STEP 2: Check if spell resists.
    ------------------------------
    -- Check the amount of Song+ and All_Song+ gear.
    local gearBoost = caster:getMod(pTable[spellId][column.SONG_MODIFIER]) + caster:getMod(xi.mod.ALL_SONGS_EFFECT)

    -- Finale has innate +175 to magic accuracy.
    local bonusMagicAcc = 0
    if spellEffect == xi.effect.NONE then
        bonusMagicAcc = xi.spells.enfeebling.finaleBonusMacc(gearBoost)
    end

    local resistRate = xi.combat.magicHitRate.calculateResistRate(caster, target, xi.magic.spellGroup.SONG, xi.skill.SINGING, 0, spellElement, xi.mod.CHR, spellEffect, bonusMagicAcc)
    if not xi.data.statusEffect.isResistRateSuccessfull(spellEffect, resistRate, 0) then
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)
        return spellEffect
    end

    if
        spellEffect == xi.effect.CHARM_I and
        (not target:isMob() or
        target:getMobMod(xi.mobMod.CHARMABLE) <= 0)
    then
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)
        return spellEffect
    end

    ------------------------------
    -- STEP 3: Calculate power, tick, duration and subEffect.
    ------------------------------
    local power     = xi.spells.enfeebling.calculateSongPower(caster, spellEffect, pTable[spellId][column.SONG_POWER_BASE], gearBoost) or 0
    local tick      = xi.spells.enfeebling.requiemTickFor(spellEffect)
    local duration  = xi.spells.enfeebling.calculateSongDuration(caster, spellEffect, pTable[spellId][column.SONG_DURATION], gearBoost) or 0
    local subEffect = spellEffect == xi.effect.THRENODY and xi.data.element.getElementalMEVAModifier(xi.data.element.getElementStrength(spellElement)) or 0

    -- FClamp and floor (pure clampSongPower).
    power    = xi.spells.enfeebling.clampSongPower(power, pTable[spellId][column.SONG_POWER_CAP])
    duration = math.floor(duration * resistRate)

    ------------------------------
    -- STEP 4: Special cases.
    ------------------------------
    -- Finale doesn't apply a debuff. Quit early.
    if spellEffect == xi.effect.NONE then
        -- TODO: This is actually message 342 which doesn't exist currently. The wording is identical.
        spell:setMsg(xi.msg.basic.MAGIC_ERASE)
        local dispelledEffect = target:dispelStatusEffect()
        if dispelledEffect == xi.effect.NONE then
            spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
        end

        return dispelledEffect

    -- Virelai applies a charm. Quit early.
    elseif spellEffect == xi.effect.CHARM_I then
        target:addStatusEffect(xi.effect.CHARM_I, { duration = duration, origin = caster })
        caster:charm(target)
        spell:setMsg(xi.spells.enfeebling.virelaiMessage(caster:isPC()))

        return spellEffect
    end

    ------------------------------
    -- STEP 5: Attempt to apply the status effect. Check for magic burst.
    ------------------------------
    if target:addStatusEffect(spellEffect, { power = power, duration = duration, origin = caster, tick = tick, subPower = subEffect, tier = spellTier }) then
        local _, skillchainCount = xi.magicburst.formMagicBurst(target, spellElement)
        if skillchainCount > 0 then
            spell:setMsg(xi.spells.enfeebling.successMessageFromParams({
                spellEffect = spellEffect, skillchainCount = skillchainCount,
            }))
            caster:triggerRoeEvent(xi.roeTrigger.MAGIC_BURST)
        else
            spell:setMsg(xi.spells.enfeebling.successMessageFromParams({
                spellEffect = spellEffect, skillchainCount = 0,
            }))
        end
    else
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
    end

    return spellEffect
end
