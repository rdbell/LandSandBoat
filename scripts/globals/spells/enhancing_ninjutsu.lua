-----------------------------------
-- Enhancing Ninjutsu Spell Utilities
-- Dual-wired pure inject forms (slice 6724 / 0879 / 6116):
--   lookupNinjutsuSpell, calculateNinjutsuPowerFromParams,
--   useEnhancingNinjutsuFromParams, ninjutsuSpellCatalogSize
-- Parity: internal/enhancingninjutsu
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.enhancing = xi.spells.enhancing or {}
-----------------------------------

-- Pins matching internal/enhancingninjutsu.
xi.spells.enhancing.ninjutsuMigawariSubPower        = 100
xi.spells.enhancing.ninjutsuUtsusemiSubPowerThreshold = 3
xi.spells.enhancing.ninjutsuUtsusemiDurationSeconds = 900
xi.spells.enhancing.ninjutsuSneakInvisibleTick      = 10
xi.spells.enhancing.ninjutsuMsgMagicNoEffect        = 75
xi.spells.enhancing.ninjutsuMsgMagicGainEffect      = 230

local column =
{
    EFFECT_TIER           = 1,
    EFFECT_ID             = 2,
    EFFECT_POWER          = 3,
    EFFECT_DURATION       = 4,
    EFFECT_WILL_OVERWRITE = 5,
}
-- Table variables.
local pTable =
{
-- Structure:            [spellId] = { Tier, Main_Effect, Power, Duration, Always_Overwrite },
    [xi.magic.spell.GEKKA_ICHI   ] = { 1, xi.effect.ENMITY_BOOST,     30, 300, true  },
    [xi.magic.spell.KAKKA_ICHI   ] = { 1, xi.effect.STORE_TP,         10, 180, true  },
    [xi.magic.spell.MIGAWARI_ICHI] = { 1, xi.effect.MIGAWARI,          0,  60, true  },
    [xi.magic.spell.MONOMI_ICHI  ] = { 1, xi.effect.SNEAK,             0, 420, false },
    [xi.magic.spell.MYOSHU_ICHI  ] = { 1, xi.effect.SUBTLE_BLOW_PLUS, 10, 180, true  },
    [xi.magic.spell.TONKO_ICHI   ] = { 1, xi.effect.INVISIBLE,         0, 420, false },
    [xi.magic.spell.TONKO_NI     ] = { 2, xi.effect.INVISIBLE,         0, 600, false },
    [xi.magic.spell.UTSUSEMI_ICHI] = { 1, xi.effect.COPY_IMAGE,        3,   0, false },
    [xi.magic.spell.UTSUSEMI_NI  ] = { 1, xi.effect.COPY_IMAGE,        4,   0, false },
    [xi.magic.spell.UTSUSEMI_SAN ] = { 1, xi.effect.COPY_IMAGE,        5,   0, false },
    [xi.magic.spell.YAIN_ICHI    ] = { 1, xi.effect.PAX,              15, 300, true  },
}

-- Pure catalog lookup (internal/enhancingninjutsu.Lookup).
-- returns: { tier, effect, power, duration, alwaysOverwrite } or nil
xi.spells.enhancing.lookupNinjutsuSpell = function(spellId)
    local row = pTable[spellId or 0]
    if not row then
        return nil
    end

    return {
        tier            = row[column.EFFECT_TIER],
        effect          = row[column.EFFECT_ID],
        power           = row[column.EFFECT_POWER],
        duration        = row[column.EFFECT_DURATION],
        alwaysOverwrite = row[column.EFFECT_WILL_OVERWRITE],
    }
end

xi.spells.enhancing.ninjutsuSpellCatalogSize = function()
    local n = 0
    for _ in pairs(pTable) do
        n = n + 1
    end

    return n
end

-- Pure calculateNinjutsuPower (internal/enhancingninjutsu.Power).
-- params: spellId, spellEffect, tablePower, ninjutsuSkill, utsusemiBonus, mainJob
-- returns: power, subPower
xi.spells.enhancing.calculateNinjutsuPowerFromParams = function(params)
    params = params or {}
    local power    = params.tablePower or 0
    local subPower = 0
    local spellEffect = params.spellEffect or 0

    -- Migawari
    if spellEffect == xi.effect.MIGAWARI then
        power    = math.floor((params.ninjutsuSkill or 0) / 5)
        subPower = xi.spells.enhancing.ninjutsuMigawariSubPower

    -- Utsusemi
    elseif spellEffect == xi.effect.COPY_IMAGE then
        power    = power + (params.utsusemiBonus or 0)
        subPower = xi.effect.COPY_IMAGE_3

        -- Utsusemi: Ni non-ninja penalty
        if
            params.spellId == xi.magic.spell.UTSUSEMI_NI and
            (params.mainJob or 0) ~= xi.job.NIN
        then
            power = power - 1
        end

        if power > xi.spells.enhancing.ninjutsuUtsusemiSubPowerThreshold then
            subPower = subPower + 1
        end
    end

    return power, subPower
end

-- Pure useEnhancingNinjutsu plan (internal/enhancingninjutsu.Use).
-- params: spellEffect, duration, alwaysOverwrite, tablePower, power, subPower,
--         hasThirdEye, hasCopyImage, existingCopyImagePower, addStatusOK
-- returns plan table with apply/del/msg fields
xi.spells.enhancing.useEnhancingNinjutsuFromParams = function(params)
    params = params or {}
    local effect     = params.spellEffect or 0
    local paramThree = 0
    local res =
    {
        returnEffect   = effect,
        setMsg         = false,
        msg            = 0,
        delPax         = false,
        delEnmityBoost = false,
        delSpellEffect = false,
        delThirdEye    = false,
        applyEffect    = false,
        power          = params.power or 0,
        duration       = params.duration or 0,
        tick           = 0,
        subPower       = params.subPower or 0,
        icon           = 0,
    }

    -- Exception pre-deletes / tick
    if effect == xi.effect.ENMITY_BOOST then
        res.delPax = true
    elseif effect == xi.effect.SNEAK or effect == xi.effect.INVISIBLE then
        paramThree = xi.spells.enhancing.ninjutsuSneakInvisibleTick
    elseif effect == xi.effect.PAX then
        res.delEnmityBoost = true
    end

    -- alwaysOverwrite: silent del+add
    if params.alwaysOverwrite then
        res.delSpellEffect = true
        res.applyEffect    = true
        res.tick           = paramThree
        return res
    end

    -- Utsusemi exception
    if effect == xi.effect.COPY_IMAGE then
        if params.hasThirdEye then
            res.delThirdEye = true
        end

        paramThree = (params.tablePower or 0) - 2
        -- Effect fields are swapped relative to Power product:
        --   power  = paramThree (table shadows − 2)
        --   subPower = calculated power (with gear)
        --   icon = calculated subPower (COPY_IMAGE_3/4)
        res.power    = paramThree
        res.duration = xi.spells.enhancing.ninjutsuUtsusemiDurationSeconds
        res.tick     = params.duration or 0
        res.icon     = params.subPower or 0
        res.subPower = params.power or 0
        res.setMsg   = true

        if not params.hasCopyImage or (params.existingCopyImagePower or 0) <= paramThree then
            res.applyEffect = true
            res.msg         = xi.spells.enhancing.ninjutsuMsgMagicGainEffect
            return res
        end

        res.msg = xi.spells.enhancing.ninjutsuMsgMagicNoEffect
        return res
    end

    -- Default addStatusEffect
    res.tick   = paramThree
    res.setMsg = true
    if params.addStatusOK then
        res.applyEffect = true
        res.msg         = xi.spells.enhancing.ninjutsuMsgMagicGainEffect
        return res
    end

    res.msg = xi.spells.enhancing.ninjutsuMsgMagicNoEffect
    return res
end

-- Ninjutsu Potency function (host → pure).
xi.spells.enhancing.calculateNinjutsuPower = function(caster, target, spell, spellId, tier, spellEffect)
    local tablePower = pTable[spellId][column.EFFECT_POWER]

    return xi.spells.enhancing.calculateNinjutsuPowerFromParams({
        spellId       = spellId,
        spellEffect   = spellEffect,
        tablePower    = tablePower,
        ninjutsuSkill = caster:getSkillLevel(xi.skill.NINJUTSU),
        utsusemiBonus = target:getMod(xi.mod.UTSUSEMI_BONUS),
        mainJob       = caster:getMainJob(),
    })
end

-- Main function for Enhancing Ninjutsu (host → pure plan).
xi.spells.enhancing.useEnhancingNinjutsu = function(caster, target, spell)
    local spellId = spell:getID()
    local entry   = xi.spells.enhancing.lookupNinjutsuSpell(spellId)
    if not entry then
        return 0
    end

    local tier            = entry.tier
    local spellEffect     = entry.effect
    local duration        = entry.duration
    local alwaysOverwrite = entry.alwaysOverwrite
    local tablePower      = entry.power

    local power, subPower = xi.spells.enhancing.calculateNinjutsuPower(caster, target, spell, spellId, tier, spellEffect)

    local hasCopyImage = false
    local existingPower = 0
    local targetEffect  = target:getStatusEffect(xi.effect.COPY_IMAGE)
    if targetEffect ~= nil then
        hasCopyImage  = true
        existingPower = targetEffect:getPower()
    end

    -- addStatusOK only used for non-alwaysOverwrite non-Utsusemi path;
    -- for pure plan we probe intent after exceptions. Host applies via status API.
    local plan = xi.spells.enhancing.useEnhancingNinjutsuFromParams({
        spellEffect            = spellEffect,
        duration               = duration,
        alwaysOverwrite        = alwaysOverwrite,
        tablePower             = tablePower,
        power                  = power,
        subPower               = subPower,
        hasThirdEye            = target:hasStatusEffect(xi.effect.THIRD_EYE),
        hasCopyImage           = hasCopyImage,
        existingCopyImagePower = existingPower,
        addStatusOK            = true, -- optimistic; host falls back on add failure
    })

    if plan.delPax then
        target:delStatusEffect(xi.effect.PAX)
    end

    if plan.delEnmityBoost then
        target:delStatusEffect(xi.effect.ENMITY_BOOST)
    end

    if plan.delThirdEye then
        target:delStatusEffect(xi.effect.THIRD_EYE)
    end

    if plan.delSpellEffect then
        target:delStatusEffect(spellEffect)
    end

    if plan.applyEffect then
        if spellEffect == xi.effect.COPY_IMAGE and not alwaysOverwrite then
            target:addStatusEffect(xi.effect.COPY_IMAGE, {
                power    = plan.power,
                duration = plan.duration,
                origin   = caster,
                tick     = plan.tick,
                icon     = plan.icon,
                subPower = plan.subPower,
            })
        else
            local ok = true
            if alwaysOverwrite then
                target:addStatusEffect(spellEffect, {
                    power    = plan.power,
                    duration = plan.duration,
                    origin   = caster,
                    tick     = plan.tick,
                    subPower = plan.subPower,
                })
            else
                ok = target:addStatusEffect(spellEffect, {
                    power    = plan.power,
                    duration = plan.duration,
                    origin   = caster,
                    tick     = plan.tick,
                    subPower = plan.subPower,
                })
                if not ok then
                    -- Recompute default-path message for add failure.
                    plan = xi.spells.enhancing.useEnhancingNinjutsuFromParams({
                        spellEffect     = spellEffect,
                        duration        = duration,
                        alwaysOverwrite = alwaysOverwrite,
                        tablePower      = tablePower,
                        power           = power,
                        subPower        = subPower,
                        addStatusOK     = false,
                    })
                end
            end
        end
    end

    if plan.setMsg then
        spell:setMsg(plan.msg)
    end

    return spellEffect
end
