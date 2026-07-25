-----------------------------------
-- Aftermath handling
-----------------------------------
xi = xi or {}

xi.aftermath = {}

xi.aftermath.type =
{
    RELIC    = 1,
    MYTHIC   = 2,
    EMPYREAN = 3,
} -- TODO: Add Aeonic

-----------------------------------
-- HELPERS : Dual-wired pure (slice 6726 / 0878)
-- Parity: internal/aftermath
-----------------------------------
xi.aftermath.getTier1RelicDuration = function(tp)
    return math.floor((tp or 0) * 0.02)
end

xi.aftermath.getTier2RelicDuration = function(tp)
    return math.floor((tp or 0) * 0.06)
end

-- Local aliases so existing effects table rows keep working.
local getTier1RelicDuration = xi.aftermath.getTier1RelicDuration
local getTier2RelicDuration = xi.aftermath.getTier2RelicDuration

-- Effect ID pins (internal/aftermath).
xi.aftermath.effectAftermath    = 273
xi.aftermath.effectAftermathLV1 = 270
xi.aftermath.effectAftermathLV2 = 271
xi.aftermath.effectAftermathLV3 = 272

xi.aftermath.levelFromTP = function(tp)
    return math.floor((tp or 0) / 1000)
end

xi.aftermath.levelIcon = function(level)
    if level == 1 then
        return xi.aftermath.effectAftermathLV1
    elseif level == 2 then
        return xi.aftermath.effectAftermathLV2
    elseif level == 3 then
        return xi.aftermath.effectAftermathLV3
    end

    return xi.aftermath.effectAftermath
end

xi.aftermath.validID = function(typ, id)
    id = id or 0
    if typ == xi.aftermath.type.RELIC then
        return id >= 1 and id <= 28
    elseif typ == xi.aftermath.type.MYTHIC then
        return id >= 29 and id <= 43
    elseif typ == xi.aftermath.type.EMPYREAN then
        return id >= 44 and id <= 45
    end

    return false
end

xi.aftermath.includePets = function(id)
    return id == 19 -- Guttler T2
end

xi.aftermath.clearsSpikes = function(id)
    return id == 8 or id == 22 -- Gungnir T1/T2
end

xi.aftermath.relicDurationSeconds = function(id, tp)
    id = id or 0
    tp = tp or 0
    if id >= 1 and id <= 14 then
        return xi.aftermath.getTier1RelicDuration(tp)
    elseif id >= 15 and id <= 28 then
        return xi.aftermath.getTier2RelicDuration(tp)
    end

    return nil
end

xi.aftermath.effects =
{
    -----------------------------------
    -- Tier 1 Relic
    -----------------------------------
    [1]  = { mods = { xi.mod.SUBTLE_BLOW, 10 }, duration = getTier1RelicDuration }, -- Spharai
    [2]  = { mods = { xi.mod.CRITHITRATE, 5 }, duration = getTier1RelicDuration }, -- Mandau
    [3]  = { mods = { xi.mod.REGEN, 10 }, duration = getTier1RelicDuration }, -- Excalibur
    [4]  = { mods = { xi.mod.CRITHITRATE, 5 }, duration = getTier1RelicDuration }, -- Ragnarok
    [5]  = { mods = { xi.mod.ATTP, 10 }, duration = getTier1RelicDuration }, -- Guttler
    [6]  = { mods = { xi.mod.DMG, -2000 }, duration = getTier1RelicDuration }, -- Bravura
    [7]  = { mods = { xi.mod.HASTE_GEAR, 1000 }, duration = getTier1RelicDuration }, -- Apocalypse
    [8]  = { mods = { xi.mod.SPIKES, xi.subEffect.SHOCK_SPIKES, xi.mod.SPIKES_DMG, 10 }, duration = getTier1RelicDuration }, -- Gungnir
    [9]  = { mods = { xi.mod.SUBTLE_BLOW, 10 }, duration = getTier1RelicDuration }, -- Kikoku
    [10] = { mods = { xi.mod.STORETP, 7 }, duration = getTier1RelicDuration }, -- Amanomurakumo
    [11] = { mods = { xi.mod.ACC, 20 }, duration = getTier1RelicDuration }, -- Mjollnir
    [12] = { mods = { xi.mod.REFRESH, 8 }, duration = getTier1RelicDuration }, -- Claustrum
    [13] = { mods = { xi.mod.RACC, 20 }, duration = getTier1RelicDuration }, -- Yoichinoyumi
    [14] = { mods = { xi.mod.ENMITY, -20 }, duration = getTier1RelicDuration }, -- Annihilator

    -----------------------------------
    -- Tier 2 Relic
    -----------------------------------
    [15] = { mods = { xi.mod.SUBTLE_BLOW, 10, xi.mod.KICK_ATTACK_RATE, 15 }, duration = getTier2RelicDuration }, -- Spharai
    [16] = { mods = { xi.mod.CRITHITRATE, 5, xi.mod.CRIT_DMG_INCREASE, 5 }, duration = getTier2RelicDuration }, -- Mandau
    [17] = { mods = { xi.mod.REGEN, 30, xi.mod.REFRESH, 3 }, duration = getTier2RelicDuration }, -- Excalibur
    [18] = { mods = { xi.mod.CRITHITRATE, 10, xi.mod.ACC, 15 }, duration = getTier2RelicDuration }, -- Ragnarok
    [19] = { mods = { xi.mod.ATTP, 10 }, duration = getTier2RelicDuration, includePets = true }, -- Guttler
    [20] = { mods = { xi.mod.DMG, -2000, xi.mod.REGEN, 15 }, duration = getTier2RelicDuration }, -- Bravura
    [21] = { mods = { xi.mod.HASTE_ABILITY, 1000, xi.mod.ACC, 15 }, duration = getTier2RelicDuration }, -- Apocalypse
    [22] = { mods = { xi.mod.SPIKES, xi.subEffect.SHOCK_SPIKES, xi.mod.SPIKES_DMG, 19, xi.mod.ATTP, 5, xi.mod.DOUBLE_ATTACK, 5 }, duration = getTier2RelicDuration }, -- Gungir
    [23] = { mods = { xi.mod.SUBTLE_BLOW, 10, xi.mod.ATTP, 10 }, duration = getTier2RelicDuration }, -- Kikoku
    [24] = { mods = { xi.mod.STORETP, 10, xi.mod.ZANSHIN, 10 }, duration = getTier2RelicDuration }, -- Amanomurakumo
    [25] = { mods = { xi.mod.ACC, 20, xi.mod.MACC, 20, xi.mod.REFRESH, 5 }, duration = getTier2RelicDuration }, -- Mjollnir
    [26] = { mods = { xi.mod.REFRESH, 15, xi.mod.DMG, -2000 }, duration = getTier2RelicDuration }, -- Claustrum
    [27] = { mods = { xi.mod.RACC, 30, xi.mod.SNAPSHOT, 5 }, duration = getTier2RelicDuration }, -- Yoichinoyumi
    [28] = { mods = { xi.mod.ENMITY, -25, xi.mod.RATTP, 10 }, duration = getTier2RelicDuration }, -- Annihilator

    -----------------------------------
    -- Tier 1 Mythic
    -----------------------------------
    [29] = -- Conqueror, Glanzfaust, Vajra, Burtgang, Liberator, Aymur, Kogarasumaru, Nagi, Ryunohige, Nirvana, Kenkonken, Terpsichore
    {
        mods =
        {
            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(tp / 100)
                end
            },

            {
                xi.mod.ATT,
                function(tp)
                    return math.floor(2 * tp / 50 - 60)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end
            }
        },

        duration = { 60, 90, 120 },
    },

    [30] = -- Yagrush, Carnwenhan
    {
        mods =
        {
            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(tp / 100)
                end
            },

            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(tp / 100 - 10)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end
            },
        },

        duration = { 180, 90, 120 },
    },

    [31] = -- Laevateinn, Murgleis, Tupsimati
    {
        mods =
        {
            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(tp / 100)
                end
            },

            {
                xi.mod.MATT,
                function(tp)
                    return math.floor(tp / 100)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end
            }
        },

        duration = { 180, 180, 120 },
    },

    [32] = -- Tizona
    {
        mods =
        {
            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(tp / 100)
                end
            },

            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(tp / 100 - 10)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end
            }
        },

        duration = { 60, 90, 120 },
    },

    [33] = -- Gastraphetes, Death Penalty
    {
        mods =
        {
            {
                xi.mod.RACC,
                function(tp)
                    return math.floor(tp / 100)
                end
            },

            {
                xi.mod.RATT,
                function(tp)
                    return math.floor(2 * tp / 50 - 60)
                end
            },

            {
                xi.mod.REM_OCC_DO_DOUBLE_DMG_RANGED,
                function(tp)
                    return 40
                end
            }
        },

        duration = { 60, 90, 120 },
    },

    -----------------------------------
    -- Tier 2 Mythic
    -----------------------------------
    [34] = -- Conqueror, Glanzfaust, Vajra, Burtgang, Liberator, Aymur, Kogarasumaru, Nagi, Ryunohige, Nirvana, Kenkonken, Terpsichore
    {
        mods =
        {
            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(3 * tp / 200)
                end
            },

            {
                xi.mod.ATT,
                function(tp)
                    return math.floor(3 * tp / 50 - 90)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 60
                end
            }
        },

        duration = { 90, 120, 180 },
    },

    [35] = -- Yagrush, Carnwenhan
    {
        mods =
        {
            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(3 * tp / 200)
                end
            },

            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(3 * tp / 200 - 15)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 60
                end
            }
        },

        duration = { 270, 120, 180 },
    },

    [36] = -- Laevateinn, Murgleis, Tupsimati
    {
        mods =
        {
            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(3 * tp / 200)
                end
            },

            {
                xi.mod.MATT,
                function(tp)
                    return math.floor(tp / 50 - 20)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 60
                end
            }
        },

        duration = { 270, 270, 180 },
    },

    [37] = -- Tizona
    {
        mods =
        {
            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(3 * tp / 200)
                end
            },

            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(3 * tp / 200 - 15)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 60
                end
            }
        },

        duration = { 90, 120, 180 },
    },

    [38] = -- Gastraphetes, Death Penalty
    {
        mods =
        {
            {
                xi.mod.RACC,
                function(tp)
                    return math.floor(tp / 50)
                end
            },

            {
                xi.mod.RATT,
                function(tp)
                    return math.floor(3 * tp / 50 - 90)
                end
            },

            {
                xi.mod.REM_OCC_DO_DOUBLE_DMG_RANGED,
                function(tp)
                    return 60
                end
            }
        },

        duration = { 90, 120, 180 },
    },

    -----------------------------------
    -- Tier 3 Mythic
    -----------------------------------
    [39] = -- Conqueror, Glanzfaust, Vajra, Burtgang, Liberator, Aymur, Kogarasumaru, Nagi, Ryunohige, Nirvana, Kenkonken, Terpsichore
    {
        mods =
        {
            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(tp / 50 + 10)
                end
            },

            {
                xi.mod.ATT,
                function(tp)
                    return math.floor(tp * 0.6 - 80)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end,

                xi.mod.MYTHIC_OCC_ATT_THRICE,
                function(tp)
                    return 20
                end
            }
        },

        duration = { 90, 120, 180 },
    },

    [40] = -- Yagrush, Carnwenhan
    {
        mods =
        {
            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(tp / 50 + 10)
                end
            },

            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(tp / 50 - 10)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end,

                xi.mod.MYTHIC_OCC_ATT_THRICE,
                function(tp)
                    return 20
                end
            }
        },

        duration = { 270, 120, 180 },
    },

    [41] = -- Laevateinn, Murgleis, Tupsimati
    {
        mods =
        {
            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(tp / 50 + 10)
                end
            },

            {
                xi.mod.MATT,
                function(tp)
                    return math.floor(tp / 50 - 10)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end,

                xi.mod.MYTHIC_OCC_ATT_THRICE,
                function(tp)
                    return 20
                end
            }
        },

        duration = { 270, 270, 180 },
    },

    [42] = -- Tizona
    {
        mods =
        {
            {
                xi.mod.ACC,
                function(tp)
                    return math.floor(tp / 50 + 10)
                end
            },

            {
                xi.mod.MACC,
                function(tp)
                    return math.floor(tp / 50 - 10)
                end
            },

            {
                xi.mod.MYTHIC_OCC_ATT_TWICE,
                function(tp)
                    return 40
                end,

                xi.mod.MYTHIC_OCC_ATT_THRICE,
                function(tp)
                    return 20
                end
            }
        },

        duration = { 90, 120, 180 },
    },

    [43] = -- Gastraphetes, Death Penalty
    {
        mods =
        {
            {
                xi.mod.RACC,
                function(tp)
                    return math.floor(tp / 50 + 10)
                end
            },

            {
                xi.mod.RATT,
                function(tp)
                    return math.floor(tp * 0.6 - 80)
                end
            },

            {
                xi.mod.REM_OCC_DO_DOUBLE_DMG_RANGED,
                function(tp)
                    return 40
                end,

                xi.mod.REM_OCC_DO_TRIPLE_DMG_RANGED,
                function(tp)
                    return 20
                end
            }
        },

        duration = { 90, 120, 180 },
    },

    -----------------------------------
    -- Tier 1 Empyrean
    -----------------------------------
    [44] =
    {
        mod = xi.mod.REM_OCC_DO_DOUBLE_DMG,
        power = { 300, 400, 500 }, -- 30%, 40%, 50% in core, fetched with rate = (x / 10)
        duration = { 30, 60, 90 },
    },

    -----------------------------------
    -- Tier 2 Empyrean
    -----------------------------------
    [45] =
    {
        mod = xi.mod.REM_OCC_DO_TRIPLE_DMG,
        power = { 300, 400, 500 }, -- 30%, 40%, 50% in core, fetched with rate = (x / 10)
        duration = { 60, 120, 180 },
    }
}

-----------------------------------
-- Pure inject forms (slice 6726)
-----------------------------------

-- Pure duration seconds (internal/aftermath.DurationSeconds).
-- params: aftermathType, id, tp
-- returns seconds or nil
xi.aftermath.durationSecondsFromParams = function(params)
    params = params or {}
    local typ = params.aftermathType or 0
    local id  = params.id or 0
    local tp  = params.tp or 0

    if typ == xi.aftermath.type.RELIC then
        return xi.aftermath.relicDurationSeconds(id, tp)
    end

    local row = xi.aftermath.effects[id]
    if not row or type(row.duration) ~= 'table' then
        return nil
    end

    local level = xi.aftermath.levelFromTP(tp)
    if level < 1 or level > 3 then
        return nil
    end

    return row.duration[level]
end

-- Pure empyrean power ladder (internal/aftermath.EmpyreanPower).
xi.aftermath.empyreanPowerFromParams = function(params)
    params = params or {}
    local id    = params.id or 0
    local level = params.level or 0
    local row   = xi.aftermath.effects[id]
    if not row or not row.power or level < 1 or level > 3 then
        return nil
    end

    return row.power[level]
end

-- Pure canOverwrite (internal/aftermath.CanOverwrite).
-- params: hasEffect, existingTier, existingSubPower, existingTimeRemainingMs,
--         newType, newID, newTP
xi.aftermath.canOverwriteFromParams = function(params)
    params = params or {}
    if not params.hasEffect then
        return true
    end

    local newType = params.newType or 0
    if newType < (params.existingTier or 0) then
        return false
    end

    if newType == xi.aftermath.type.RELIC then
        local secs = xi.aftermath.durationSecondsFromParams({
            aftermathType = xi.aftermath.type.RELIC,
            id            = params.newID or 0,
            tp            = params.newTP or 0,
        })
        if not secs then
            return false
        end

        return secs * 1000 > (params.existingTimeRemainingMs or 0)
    elseif newType == xi.aftermath.type.MYTHIC or newType == xi.aftermath.type.EMPYREAN then
        local currentLevel = xi.aftermath.levelFromTP(params.existingSubPower or 0)
        local newLevel     = xi.aftermath.levelFromTP(params.newTP or 0)
        return currentLevel == 1 or currentLevel < newLevel
    end

    return false
end

-- Pure addStatusEffect plan (internal/aftermath.AddStatusEffect).
-- params: isPC, hasWeapon, aftermathID, aftermathType, tp,
--         hasEffect, existingTier, existingSubPower, existingTimeRemainingMs
-- returns: { ok, delAftermath, clearSpikes, power, duration, subPower, tier, icon, hasIcon }
xi.aftermath.addStatusEffectFromParams = function(params)
    params = params or {}
    if not params.isPC or not params.hasWeapon then
        return { ok = false }
    end

    local typ = params.aftermathType or 0
    local id  = params.aftermathID or 0
    local tp  = params.tp or 0

    if not xi.aftermath.validID(typ, id) then
        return { ok = false }
    end

    local dur = xi.aftermath.durationSecondsFromParams({
        aftermathType = typ,
        id            = id,
        tp            = tp,
    })
    if not dur then
        return { ok = false }
    end

    if not xi.aftermath.canOverwriteFromParams({
        hasEffect               = params.hasEffect,
        existingTier            = params.existingTier,
        existingSubPower        = params.existingSubPower,
        existingTimeRemainingMs = params.existingTimeRemainingMs,
        newType                 = typ,
        newID                   = id,
        newTP                   = tp,
    }) then
        return { ok = false }
    end

    local res =
    {
        ok           = true,
        delAftermath = true,
        clearSpikes  = false,
        power        = id,
        duration     = dur,
        subPower     = tp,
        tier         = typ,
        icon         = 0,
        hasIcon      = false,
    }

    if typ == xi.aftermath.type.RELIC and xi.aftermath.clearsSpikes(id) then
        res.clearSpikes = true
    end

    if typ == xi.aftermath.type.MYTHIC or typ == xi.aftermath.type.EMPYREAN then
        res.hasIcon = true
        res.icon    = xi.aftermath.levelIcon(xi.aftermath.levelFromTP(tp))
    end

    return res
end

xi.aftermath.addStatusEffect = function(player, tp, weaponSlot, aftermathType)
    -- Players only! Host gathers injects then pure plan applies.
    local isPC = player:getObjType() == xi.objType.PC
    local weapon = isPC and player:getStorageItem(0, 0, weaponSlot) or nil
    local id = weapon and weapon:getMod(xi.mod.AFTERMATH) or 0

    local hasEffect = false
    local existingTier = 0
    local existingSubPower = 0
    local existingTimeRemainingMs = 0
    local effect = player:getStatusEffect(xi.effect.AFTERMATH)
    if effect then
        hasEffect = true
        existingTier = effect:getTier()
        existingSubPower = effect:getSubPower()
        existingTimeRemainingMs = effect:getTimeRemaining()
    end

    local plan = xi.aftermath.addStatusEffectFromParams({
        isPC                    = isPC,
        hasWeapon               = weapon ~= nil,
        aftermathID             = id,
        aftermathType           = aftermathType,
        tp                      = tp,
        hasEffect               = hasEffect,
        existingTier            = existingTier,
        existingSubPower        = existingSubPower,
        existingTimeRemainingMs = existingTimeRemainingMs,
    })

    if not plan.ok then
        return
    end

    if plan.delAftermath then
        player:delStatusEffect(xi.effect.AFTERMATH)
    end

    if plan.clearSpikes then
        -- Gungnir's AM overwrites and prevents all Spikes spells from landing. Core handles the latter
        player:delStatusEffectsByType(xi.effectType.SPIKES)
    end

    if plan.hasIcon then
        player:addStatusEffect(xi.effect.AFTERMATH, {
            power    = plan.power,
            duration = plan.duration,
            origin   = player,
            icon     = plan.icon,
            subPower = plan.subPower,
            tier     = plan.tier,
        })
    else
        player:addStatusEffect(xi.effect.AFTERMATH, {
            power    = plan.power,
            duration = plan.duration,
            origin   = player,
            subPower = plan.subPower,
            tier     = plan.tier,
        })
    end
end

-----------------------------------
-- Effect Power = Aftermath ID
-- Effect SubPower = TP
-- Effect Tier = Aftermath Type
-----------------------------------
xi.aftermath.onEffectGain = function(target, effect)
    local aftermath = xi.aftermath.effects[effect:getPower()]
    switch (effect:getTier()) : caseof
    {
        [xi.aftermath.type.RELIC] = function(x)
            local pet = target:getPet()
            if
                pet and
                aftermath.includePets
            then
                -- pets gain same mods as the player, so give them the effect without a loss message
                pet:delStatusEffectSilent(xi.effect.AFTERMATH)
                pet:addStatusEffect(xi.effect.AFTERMATH, { power = effect:getPower(), duration = effect:getDuration() / 1000, origin = target, subPower = effect:getSubPower(), tier = effect:getTier() })
                pet:getStatusEffect(xi.effect.AFTERMATH):addEffectFlag(xi.effectFlag.NO_LOSS_MESSAGE)
            end

            for i = 1, #aftermath.mods, 2 do
                effect:addMod(aftermath.mods[i], aftermath.mods[i + 1])
            end
        end,

        [xi.aftermath.type.MYTHIC] = function(x)
            local tp = effect:getSubPower()
            local mods = aftermath.mods[math.floor(tp / 1000)]
            local pet = target:getPet()
            if pet then
                -- pets gain same mods as the player, so give them the effect without a loss message
                pet:delStatusEffectSilent(xi.effect.AFTERMATH)
                pet:addStatusEffect(xi.effect.AFTERMATH, { power = effect:getPower(), duration = effect:getDuration() / 1000, origin = target, subPower = effect:getSubPower(), tier = effect:getTier() })
                pet:getStatusEffect(xi.effect.AFTERMATH):addEffectFlag(xi.effectFlag.NO_LOSS_MESSAGE)
            end

            for i = 1, #mods, 2 do
                effect:addMod(mods[i], mods[i + 1](tp))
            end
        end,

        [xi.aftermath.type.EMPYREAN] = function(x)
            effect:addMod(aftermath.mod, aftermath.power[math.floor(effect:getSubPower() / 1000)])
        end
    }
end

xi.aftermath.canOverwrite = function(player, tp, aftermathId, aftermathType)
    -- Host → pure canOverwriteFromParams (slice 6726).
    local effect = player:getStatusEffect(xi.effect.AFTERMATH)
    if not effect then
        return xi.aftermath.canOverwriteFromParams({ hasEffect = false })
    end

    return xi.aftermath.canOverwriteFromParams({
        hasEffect               = true,
        existingTier            = effect:getTier(),
        existingSubPower        = effect:getSubPower(),
        existingTimeRemainingMs = effect:getTimeRemaining(),
        newType                 = aftermathType,
        newID                   = aftermathId,
        newTP                   = tp,
    })
end
