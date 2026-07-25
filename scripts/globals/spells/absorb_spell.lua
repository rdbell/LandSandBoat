-----------------------------------
-- Absorb Spell Utilities
-- Drain, Aspir, Absorb-TP, Absorb-STAT, Absorb-Attri
--
-- Dual-wired pure inject forms (slice 6723 / 0880):
--   absorbStatPotencyFromParams, absorbStatDurationFromParams,
--   drainAspirRangeFromParams, drainAspirProductFromParams,
--   maxHPBoostDurationFromParams, absorbTPDamageFromParams
-- Parity: internal/absorbspell
-----------------------------------
require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/spells/damage_spell')
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.absorb = xi.spells.absorb or {}
-----------------------------------

-- Pins matching internal/absorbspell clamps.
xi.spells.absorb.statDurationMin      = 0
xi.spells.absorb.statDurationMax      = 10000
xi.spells.absorb.absorbTPDamageCap    = 3000
xi.spells.absorb.skillSoftcap         = 300
xi.spells.absorb.maxHPBoostBaseDuration = 180

local absorbStatData =
{
    [xi.magic.spell.ABSORB_STR] = { boostEffect = xi.effect.STR_BOOST,      downEffect = xi.effect.STR_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_STR },
    [xi.magic.spell.ABSORB_DEX] = { boostEffect = xi.effect.DEX_BOOST,      downEffect = xi.effect.DEX_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_DEX },
    [xi.magic.spell.ABSORB_VIT] = { boostEffect = xi.effect.VIT_BOOST,      downEffect = xi.effect.VIT_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_VIT },
    [xi.magic.spell.ABSORB_AGI] = { boostEffect = xi.effect.AGI_BOOST,      downEffect = xi.effect.AGI_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_AGI },
    [xi.magic.spell.ABSORB_INT] = { boostEffect = xi.effect.INT_BOOST,      downEffect = xi.effect.INT_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_INT },
    [xi.magic.spell.ABSORB_MND] = { boostEffect = xi.effect.MND_BOOST,      downEffect = xi.effect.MND_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_MND },
    [xi.magic.spell.ABSORB_CHR] = { boostEffect = xi.effect.CHR_BOOST,      downEffect = xi.effect.CHR_DOWN,      msg = xi.msg.basic.MAGIC_ABSORB_CHR },
    [xi.magic.spell.ABSORB_ACC] = { boostEffect = xi.effect.ACCURACY_BOOST, downEffect = xi.effect.ACCURACY_DOWN, msg = xi.msg.basic.MAGIC_ABSORB_ACC },
}

local absorbPointsData =
{
    -- [spell ID] = { parameter, { skill <= 300 }, { skill > 300 }, divisor, increase max HP? }
    [xi.magic.spell.DRAIN    ] = { xi.mod.HP, {   1,  20 }, { 0.625, 132.5 }, 0.50, false },
    [xi.magic.spell.DRAIN_II ] = { xi.mod.HP, {   1, 165 }, {     1,   165 }, 0.66, true  },
    [xi.magic.spell.DRAIN_III] = { xi.mod.HP, {   1, 255 }, {   1.5,   105 }, 0.75, true  },
    [xi.magic.spell.ASPIR    ] = { xi.mod.MP, { 0.3,  20 }, {   0.4,     0 }, 0.50, false },
    [xi.magic.spell.ASPIR_II ] = { xi.mod.MP, { 0.5,  30 }, {   0.6,     0 }, 0.50, false },
    [xi.magic.spell.ASPIR_III] = { xi.mod.MP, { 0.7,  40 }, {   0.8,     0 }, 0.50, false },
}

-- Pure Absorb-STAT potency (internal/absorbspell.StatPotency).
-- params: mainLevel, augmentsAbsorb, liberator, netherVoidPower, hasNetherVoid
xi.spells.absorb.absorbStatPotencyFromParams = function(params)
    params = params or {}
    local base = 3 + math.floor((params.mainLevel or 0) / 5)
    local gear = 1 + (params.augmentsAbsorb or 0) / 100
    local lib  = 1 + (params.liberator or 0) / 100
    local final = math.floor(base * gear * lib)
    local nv = 1
    if params.hasNetherVoid then
        nv = 1 + (params.netherVoidPower or 0) / 100
    end

    return math.floor(final * nv)
end

-- Pure Absorb-STAT duration (internal/absorbspell.StatDuration).
-- params: darkSkill, darkMagicDuration, absorbEffectDuration, enhancesAbsorb
xi.spells.absorb.absorbStatDurationFromParams = function(params)
    params = params or {}
    local base = 180 + math.floor(((params.darkSkill or 0) - 490.5) / 5)
    base = utils.clamp(base, xi.spells.absorb.statDurationMin, xi.spells.absorb.statDurationMax)
    local dark = 1 + (params.darkMagicDuration or 0) / 100
    local gear = 1 + (params.absorbEffectDuration or 0) / 100

    return math.floor(base * dark * gear) + (params.enhancesAbsorb or 0)
end

-- Pure Drain/Aspir skill-equation min/max (internal/absorbspell.DrainAspirRangeForSpell).
-- params: spellId, darkSkill
-- returns: minPotential, maxPotential (or nil,nil if unknown spell)
xi.spells.absorb.drainAspirRangeFromParams = function(params)
    params = params or {}
    local row = absorbPointsData[params.spellId]
    if not row then
        return nil, nil
    end

    local skill = params.darkSkill or 0
    local skillEquation = skill > xi.spells.absorb.skillSoftcap and 3 or 2
    local maxDamagePotential = math.floor(skill * row[skillEquation][1] + row[skillEquation][2])
    local minDamagePotential = math.floor(maxDamagePotential * row[4])

    return minDamagePotential, maxDamagePotential
end

-- Pure Drain/Aspir post-base floor chain (internal/absorbspell.DrainAspirProduct).
xi.spells.absorb.drainAspirProductFromParams = function(params)
    params = params or {}
    local absorbMult = 1 + (params.augmentsAbsorb or 0) / 100 + (params.enhDrainAspir or 0) / 100
    local libMult    = 1 + (params.liberator or 0) / 100
    local nvMult     = 1
    if params.hasNetherVoid then
        nvMult = 1 + (params.netherVoidPower or 0) / 100
    end

    local dmg = params.baseDamage or 0
    dmg = math.floor(dmg * (params.resistTier or 1))
    dmg = math.floor(dmg * (params.additionalResistTier or 1))
    dmg = math.floor(dmg * (params.sdt or 1))
    dmg = math.floor(dmg * (params.elementalStaffBonus or 1))
    dmg = math.floor(dmg * (params.elementalAffinity or 1))
    dmg = math.floor(dmg * (params.dayAndWeather or 1))
    dmg = math.floor(dmg * absorbMult)
    dmg = math.floor(dmg * libMult)
    dmg = math.floor(dmg * nvMult)

    return dmg
end

-- Pure Drain II/III MAX_HP_BOOST duration (internal/absorbspell.MaxHPBoostDuration).
xi.spells.absorb.maxHPBoostDurationFromParams = function(params)
    params = params or {}
    local base = xi.spells.absorb.maxHPBoostBaseDuration

    return base + base * (params.darkMagicDuration or 0) / 100
end

-- Pure Absorb-TP damage product (internal/absorbspell.AbsorbTPDamage).
-- Note: no elemental affinity term (LSB parity).
xi.spells.absorb.absorbTPDamageFromParams = function(params)
    params = params or {}
    local base         = (params.targetTP or 0) * 30 / 100
    local absorbMult   = 1 + (params.augmentsAbsorb or 0) / 100
    local absorbTPMult = 1 + (params.augmentsAbsorbTP or 0) / 100
    local libMult      = 1 + (params.liberator or 0) / 100

    local dmg = base
    dmg = math.floor(dmg * (params.resistTier or 1))
    dmg = math.floor(dmg * (params.additionalResistTier or 1))
    dmg = math.floor(dmg * (params.sdt or 1))
    dmg = math.floor(dmg * (params.elementalStaffBonus or 1))
    dmg = math.floor(dmg * (params.dayAndWeather or 1))
    dmg = math.floor(dmg * absorbMult)
    dmg = math.floor(dmg * absorbTPMult)
    dmg = math.floor(dmg * libMult)

    return utils.clamp(dmg, 0, xi.spells.absorb.absorbTPDamageCap)
end

-- https://www.bg-wiki.com/ffxi/Category:Absorb_Spell
xi.spells.absorb.doAbsorbStatSpell = function(caster, target, spell)
    local spellId          = spell:getID()
    local enhancingEffect  = absorbStatData[spellId].boostEffect
    local enfeeblingEffect = absorbStatData[spellId].downEffect

    -- Calculate resistance (2 state effects: Either No resist, half resist or full resist)
    local resist = xi.combat.magicHitRate.calculateResistRate(caster, target, xi.magic.spellGroup.BLACK, xi.skill.DARK_MAGIC, 0, xi.element.DARK, xi.mod.INT, enfeeblingEffect, 0)
    if resist < 0.5 then
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)
        return 0
    end

    local netherVoidPower = 0
    local hasNetherVoid   = caster:hasStatusEffect(xi.effect.NETHER_VOID)
    if hasNetherVoid then
        netherVoidPower = caster:getStatusEffect(xi.effect.NETHER_VOID):getPower()
    end

    local finalPotency = xi.spells.absorb.absorbStatPotencyFromParams({
        mainLevel       = caster:getMainLvl(),
        augmentsAbsorb  = caster:getMod(xi.mod.AUGMENTS_ABSORB),
        liberator       = caster:getMod(xi.mod.AUGMENTS_ABSORB_LIBERATOR),
        netherVoidPower = netherVoidPower,
        hasNetherVoid   = hasNetherVoid,
    })

    local finalDuration = xi.spells.absorb.absorbStatDurationFromParams({
        darkSkill            = caster:getSkillLevel(xi.skill.DARK_MAGIC),
        darkMagicDuration    = caster:getMod(xi.mod.DARK_MAGIC_DURATION),
        absorbEffectDuration = caster:getMod(xi.mod.ABSORB_EFFECT_DURATION),
        enhancesAbsorb       = caster:getMod(xi.mod.ENHANCES_ABSORB_EFFECTS),
    })

    -- Apply debuff and buff if needed. Absorb effects can be overwriten via higher potency.
    if target:addStatusEffect(enfeeblingEffect, { power = finalPotency, duration = finalDuration, origin = caster }) then
        -- Set associated message.
        spell:setMsg(absorbStatData[spellId].msg)

        -- Force-overwrite associated buff.
        caster:delStatusEffect(enhancingEffect)
        caster:addStatusEffect(enhancingEffect, { power = finalPotency, duration = finalDuration, origin = caster })
    else
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
    end

    return enfeeblingEffect
end

-- https://www.bg-wiki.com/ffxi/Category:Drain/Aspir_Spell
-- https://wiki-ffo-jp.translate.goog/html/923.html?_x_tr_sl=ja&_x_tr_tl=en&_x_tr_hl=en&_x_tr_pto=sc
xi.spells.absorb.doDrainingSpell = function(caster, target, spell)
    local finalDamage  = 0
    local spellId      = spell:getID()
    local modAbsorbed  = absorbPointsData[spellId][1]
    local targetPoints = target:getHP()
    local displayCap   = caster:getMaxHP() - caster:getHP()

    if modAbsorbed == xi.mod.MP then
        targetPoints = target:getMP()
        displayCap   = caster:getMaxMP() - caster:getMP()
    end

    -- Early return: Target is undead.
    if target:isUndead() then
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
        return finalDamage
    end

    -- Early return: Target absorbs or nullifies dark.
    if
        xi.spells.damage.calculateAbsorption(target, xi.element.DARK, true) ~= 1 or
        xi.spells.damage.calculateNullification(target, xi.element.DARK, true, false) ~= 1
    then
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)
        return finalDamage
    end

    -- Early return: Target doesn't have HP/MP to absorb.
    if targetPoints == 0 then
        spell:setMsg(xi.msg.basic.NO_EFFECT)
        return finalDamage
    end

    -- Base damage via pure skill-equation range.
    local minDamagePotential, maxDamagePotential = xi.spells.absorb.drainAspirRangeFromParams({
        spellId   = spellId,
        darkSkill = caster:getSkillLevel(xi.skill.DARK_MAGIC),
    })
    local baseDamage = math.random(minDamagePotential, maxDamagePotential)

    -- Multipliers (host) → pure product chain.
    local resistTier             = xi.combat.magicHitRate.calculateResistRate(caster, target, xi.magic.spellGroup.BLACK, xi.skill.DARK_MAGIC, 0, xi.element.DARK, xi.mod.INT, 0, 0)
    local additionalResistTier   = xi.spells.damage.calculateAdditionalResistTier(caster, target, xi.element.DARK)
    local sdt                    = xi.combat.damage.magicalElementSDT(target, xi.element.DARK)
    local elementalStaffBonus    = xi.spells.damage.calculateElementalStaffBonus(caster, xi.element.DARK)
    local elementalAffinityBonus = xi.spells.damage.calculateElementalAffinityBonus(caster, xi.element.DARK)
    local dayAndWeather          = xi.spells.damage.calculateDayAndWeather(caster, xi.element.DARK, false)

    local netherVoidPower = 0
    local hasNetherVoid   = caster:hasStatusEffect(xi.effect.NETHER_VOID)
    if hasNetherVoid then
        netherVoidPower = caster:getStatusEffect(xi.effect.NETHER_VOID):getPower()
    end

    finalDamage = xi.spells.absorb.drainAspirProductFromParams({
        baseDamage           = baseDamage,
        resistTier           = resistTier,
        additionalResistTier = additionalResistTier,
        sdt                  = sdt,
        elementalStaffBonus  = elementalStaffBonus,
        elementalAffinity    = elementalAffinityBonus,
        dayAndWeather        = dayAndWeather,
        augmentsAbsorb       = caster:getMod(xi.mod.AUGMENTS_ABSORB),
        enhDrainAspir        = caster:getMod(xi.mod.ENH_DRAIN_ASPIR),
        liberator            = caster:getMod(xi.mod.AUGMENTS_ABSORB_LIBERATOR),
        netherVoidPower      = netherVoidPower,
        hasNetherVoid        = hasNetherVoid,
    })

    -- Final operations.
    if modAbsorbed == xi.mod.HP then
        finalDamage = utils.clamp(utils.handlePhalanx(target, finalDamage), 0, 99999)
        finalDamage = utils.clamp(utils.handleOneForAll(target, finalDamage), 0, 99999)
        finalDamage = utils.clamp(utils.handleStoneskin(target, finalDamage), -99999, 99999)
        finalDamage = utils.clamp(finalDamage, 0, targetPoints)
        finalDamage = target:checkDamageCap(finalDamage)

        -- Handle Bind break and TP?
        target:takeSpellDamage(caster, spell, finalDamage, xi.attackType.MAGICAL, xi.damageType.DARK)

        -- Handle Afflatus Misery.
        target:handleAfflatusMiseryDamage(finalDamage)

        -- Handle Enmity.
        target:updateEnmityFromDamage(caster, finalDamage)
    else
        finalDamage = utils.clamp(finalDamage, 0, targetPoints)
    end

    -- Drain II and Drain III increase max HP via effect.
    if absorbPointsData[spellId][5] then
        -- Remove cap on damage displayed in log.
        displayCap = 9999 - caster:getHP()

        -- Calculate overflow.
        local overflow = finalDamage + caster:getHP() - caster:getMaxHP()
        if overflow > 0 then
            -- Check if effect should be applied. Only 1 "Max HP Effect" can be in place at a time.
            -- Retail testing suggest that %power effect takes precedent over flat power.
            local hasMaxHPEffect      = caster:hasStatusEffect(xi.effect.MAX_HP_BOOST)
            local maxHPEffectPower    = 0
            local maxHPEffectSubpower = 0

            if hasMaxHPEffect then
                maxHPEffectPower    = caster:getStatusEffect(xi.effect.MAX_HP_BOOST):getPower()
                maxHPEffectSubpower = caster:getStatusEffect(xi.effect.MAX_HP_BOOST):getSubPower()
            end

            if
                not hasMaxHPEffect or           -- No effect present, so apply.
                (maxHPEffectPower == 0 and      -- Effect present, but it isn't %. If subpower is higher, we can override the effect.
                maxHPEffectSubpower < overflow) -- Subpower present is lower than new one, so we can override the effect.
            then
                local duration = xi.spells.absorb.maxHPBoostDurationFromParams({
                    darkMagicDuration = caster:getMod(xi.mod.DARK_MAGIC_DURATION),
                })
                caster:delStatusEffect(xi.effect.MAX_HP_BOOST)
                caster:addStatusEffect(xi.effect.MAX_HP_BOOST, { duration = duration, origin = caster, subPower = overflow })
            end
        end
    end

    -- Perform (non) damage and healing.
    if modAbsorbed == xi.mod.HP then
        caster:addHP(finalDamage)
    else
        caster:addMP(finalDamage)
        target:delMP(finalDamage)
    end

    -- Displayed damage in log is the amount the player heals by, not the damage actually done.
    local displayDamage = utils.clamp(finalDamage, 0, displayCap)

    return displayDamage
end

xi.spells.absorb.doAbsorbTPSpell = function(caster, target, spell)
    local finalDamage = 0

    -- Early return: Target absorbs or nullifies dark.
    if
        xi.spells.damage.calculateAbsorption(target, xi.element.DARK, true) ~= 1 or
        xi.spells.damage.calculateNullification(target, xi.element.DARK, true, false) ~= 1
    then
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)
        return finalDamage
    end

    -- Early return: Target doesn't have TP to absorb.
    local targetTP = target:getTP()
    if targetTP == 0 then
        spell:setMsg(xi.msg.basic.NO_EFFECT)
        return finalDamage
    end

    -- Multipliers (host) → pure Absorb-TP product.
    local resistTier           = xi.combat.magicHitRate.calculateResistRate(caster, target, xi.magic.spellGroup.BLACK, xi.skill.DARK_MAGIC, 0, xi.element.DARK, xi.mod.INT, 0, 0)
    local additionalResistTier = xi.spells.damage.calculateAdditionalResistTier(caster, target, xi.element.DARK)
    local sdt                  = xi.combat.damage.magicalElementSDT(target, xi.element.DARK)
    local elementalStaffBonus  = xi.spells.damage.calculateElementalStaffBonus(caster, xi.element.DARK)
    local dayAndWeather        = xi.spells.damage.calculateDayAndWeather(caster, xi.element.DARK, false)

    finalDamage = xi.spells.absorb.absorbTPDamageFromParams({
        targetTP             = targetTP,
        resistTier           = resistTier,
        additionalResistTier = additionalResistTier,
        sdt                  = sdt,
        elementalStaffBonus  = elementalStaffBonus,
        dayAndWeather        = dayAndWeather,
        augmentsAbsorb       = caster:getMod(xi.mod.AUGMENTS_ABSORB),
        augmentsAbsorbTP     = caster:getMod(xi.mod.AUGMENTS_ABSORB_TP),
        liberator            = caster:getMod(xi.mod.AUGMENTS_ABSORB_LIBERATOR),
    })

    -- Set proper message.
    spell:setMsg(xi.msg.basic.MAGIC_ABSORB_TP)

    -- Perform drain.
    caster:addTP(finalDamage)
    target:addTP(-finalDamage)

    return finalDamage
end

xi.spells.absorb.doAbsorbAttriSpell = function(caster, target, spell)
    local count       = 0
    local effectFirst = caster:stealStatusEffect(target, xi.effectFlag.DISPELABLE)

    if effectFirst ~= 0 then
        count = 1

        if caster:hasStatusEffect(xi.effect.NETHER_VOID) then
            local effectSecond = caster:stealStatusEffect(target, xi.effectFlag.DISPELABLE)
            if effectSecond ~= 0 then
                count = count + 1
            end
        end

        spell:setMsg(xi.msg.basic.MAGIC_STEAL)

        return count
    else
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT) -- No effect
    end

    return count
end
