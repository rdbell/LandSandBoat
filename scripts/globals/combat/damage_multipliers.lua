-----------------------------------
-- Damage multiplier helpers.
-- Pure injects dual-wired to OmegaXI internal/dmgmultiplier (slice 6691 / 0850 / 6075).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.damage = xi.combat.damage or {}
-----------------------------------

xi.combat.damage.sdtMin = 0
xi.combat.damage.sdtMax = 3
xi.combat.damage.combinedTakenMin = -0.5
xi.combat.damage.combinedTakenMax = 0.5
xi.combat.damage.typeIIMultMin = 0.125
xi.combat.damage.typeIIMultMax = 1.875
xi.combat.damage.uncappedMultMin = 0
xi.combat.damage.uncappedMultMax = 2
xi.combat.damage.steamJacketLocalVar = '[steamJacket]Element'

-- Physical damage-type → SDT mod map (host residual for mod-id resolution).
xi.combat.damage.physicalElementSDTModifier =
{
    [xi.damageType.PIERCING    ] = xi.mod.PIERCE_SDT,
    [xi.damageType.SLASHING    ] = xi.mod.SLASH_SDT,
    [xi.damageType.BLUNT       ] = xi.mod.IMPACT_SDT,
    [xi.damageType.HAND_TO_HAND] = xi.mod.HTH_SDT,
}

-----------------------------------
-- Pure injects
-----------------------------------

-- Pure physicalElementSDT once sdtMod is injected.
-- params: physicalElement, sdtMod (raw getMod in 1/10000 units)
xi.combat.damage.physicalElementSDTFromParams = function(params)
    local physicalElement = params.physicalElement or 0

    if
        physicalElement < xi.damageType.PIERCING or
        physicalElement > xi.damageType.HAND_TO_HAND
    then
        return 1
    end

    local sdt = 1 + (params.sdtMod or 0) / 10000

    return utils.clamp(sdt, xi.combat.damage.sdtMin, xi.combat.damage.sdtMax)
end

-- Pure magicalElementSDT once sdtMod is injected.
-- params: magicalElement, sdtMod
xi.combat.damage.magicalElementSDTFromParams = function(params)
    local magicalElement = params.magicalElement or 0

    if magicalElement < xi.element.FIRE or magicalElement > xi.element.DARK then
        return 1
    end

    local sdt = 1 + (params.sdtMod or 0) / 10000

    return utils.clamp(sdt, xi.combat.damage.sdtMin, xi.combat.damage.sdtMax)
end

-- Pure calculateDamageAdjustment once category flags and taken mods are injected.
-- All *Mod fields are raw getMod values in 1/10000 units.
-- params: isPhysical, isMagical, isRanged, isBreath,
--   dmg, dmgPhys, dmgPhysII, uDmgPhys, dmgMagic, dmgMagicII, uDmgMagic,
--   dmgRange, uDmgRange, dmgBreath, uDmgBreath
xi.combat.damage.damageAdjustmentFromParams = function(params)
    -- NOTE: -2500 -> 25% less damage taken by target. 2500 -> 25% more damage taken by target.
    local targetDamageTaken = 1

    local globalDamageTaken = (params.dmg or 0) / 10000

    local isPhysical = params.isPhysical
    local isMagical = params.isMagical
    local isRanged = params.isRanged
    local isBreath = params.isBreath

    local physicalDamageTaken         = isPhysical and (params.dmgPhys or 0) / 10000 or 0
    local physicalDamageTakenII       = isPhysical and (params.dmgPhysII or 0) / 10000 or 0
    local physicalDamageTakenUncapped = isPhysical and (params.uDmgPhys or 0) / 10000 or 0

    local magicDamageTaken            = isMagical and (params.dmgMagic or 0) / 10000 or 0
    local magicDamageTakenII          = isMagical and (params.dmgMagicII or 0) / 10000 or 0
    local magicDamageTakenUncapped    = isMagical and (params.uDmgMagic or 0) / 10000 or 0

    local rangedDamageTaken           = isRanged and (params.dmgRange or 0) / 10000 or 0
    local rangedDamageTakenUncapped   = isRanged and (params.uDmgRange or 0) / 10000 or 0

    local breathDamageTaken           = isBreath and (params.dmgBreath or 0) / 10000 or 0
    local breathDamageTakenUncapped   = isBreath and (params.uDmgBreath or 0) / 10000 or 0

    -- The combination of regular "Damage Taken" and "<type> Damage Taken" caps at 50% both ways.
    local combinedDamageTaken = utils.clamp(
        globalDamageTaken + physicalDamageTaken + magicDamageTaken + rangedDamageTaken + breathDamageTaken,
        xi.combat.damage.combinedTakenMin,
        xi.combat.damage.combinedTakenMax
    )

    -- "<type> Damage Taken II" bypasses the regular cap, but combined cap is 87.5% both ways.
    targetDamageTaken = utils.clamp(
        targetDamageTaken + combinedDamageTaken + physicalDamageTakenII + magicDamageTakenII,
        xi.combat.damage.typeIIMultMin,
        xi.combat.damage.typeIIMultMax
    )

    -- Uncapped damage modifiers. Cap is 100% both ways anyway, just in case.
    targetDamageTaken = utils.clamp(
        targetDamageTaken + physicalDamageTakenUncapped + magicDamageTakenUncapped +
            rangedDamageTakenUncapped + breathDamageTakenUncapped,
        xi.combat.damage.uncappedMultMin,
        xi.combat.damage.uncappedMultMax
    )

    return targetDamageTaken
end

-- Pure scarletDeliriumMultiplier once status gate and power are injected.
-- params: hasEffect, power
xi.combat.damage.scarletDeliriumMultiplierFromParams = function(params)
    if not params.hasEffect then
        return 1
    end

    return 1 + (params.power or 0) / 1000
end

-- Pure steamJacketMultiplier once reduction mod and prior tracked element are injected.
-- Does not write local-var (host residual).
-- params: magicalElement, reductionMod, priorTracked
xi.combat.damage.steamJacketMultiplierFromParams = function(params)
    local magicalElement = params.magicalElement or 0

    if magicalElement < xi.element.FIRE or magicalElement > xi.element.DARK then
        return 1
    end

    local reductionMod = params.reductionMod or 0

    if reductionMod <= 0 then
        return 1
    end

    if (params.priorTracked or 0) ~= magicalElement then
        return 1
    end

    return 1 - reductionMod / 100
end

-----------------------------------
-- Entity hosts (inject → pure)
-----------------------------------

xi.combat.damage.physicalElementSDT = function(target, physicalElement)
    if
        physicalElement < xi.damageType.PIERCING or
        physicalElement > xi.damageType.HAND_TO_HAND
    then
        return xi.combat.damage.physicalElementSDTFromParams({
            physicalElement = physicalElement,
        })
    end

    local modId = xi.combat.damage.physicalElementSDTModifier[physicalElement]

    return xi.combat.damage.physicalElementSDTFromParams({
        physicalElement = physicalElement,
        sdtMod          = target:getMod(modId),
    })
end

xi.combat.damage.magicalElementSDT = function(target, magicalElement)
    if magicalElement < xi.element.FIRE or magicalElement > xi.element.DARK then
        return xi.combat.damage.magicalElementSDTFromParams({
            magicalElement = magicalElement,
        })
    end

    return xi.combat.damage.magicalElementSDTFromParams({
        magicalElement = magicalElement,
        sdtMod         = target:getMod(xi.data.element.getElementalSDTModifier(magicalElement)),
    })
end

xi.combat.damage.calculateDamageAdjustment = function(target, isPhysical, isMagical, isRanged, isBreath)
    return xi.combat.damage.damageAdjustmentFromParams({
        isPhysical = isPhysical,
        isMagical  = isMagical,
        isRanged   = isRanged,
        isBreath   = isBreath,
        dmg        = target:getMod(xi.mod.DMG),
        dmgPhys    = target:getMod(xi.mod.DMGPHYS),
        dmgPhysII  = target:getMod(xi.mod.DMGPHYS_II),
        uDmgPhys   = target:getMod(xi.mod.UDMGPHYS),
        dmgMagic   = target:getMod(xi.mod.DMGMAGIC),
        dmgMagicII = target:getMod(xi.mod.DMGMAGIC_II),
        uDmgMagic  = target:getMod(xi.mod.UDMGMAGIC),
        dmgRange   = target:getMod(xi.mod.DMGRANGE),
        uDmgRange  = target:getMod(xi.mod.UDMGRANGE),
        dmgBreath  = target:getMod(xi.mod.DMGBREATH),
        uDmgBreath = target:getMod(xi.mod.UDMGBREATH),
    })
end

xi.combat.damage.scarletDeliriumMultiplier = function(actor)
    -- Scarlet delirium are 2 different status effects. SCARLET_DELIRIUM_1 is the one that boosts power.
    local hasEffect = actor:hasStatusEffect(xi.effect.SCARLET_DELIRIUM_1)
    local power = 0

    if hasEffect then
        power = actor:getStatusEffect(xi.effect.SCARLET_DELIRIUM_1):getPower()
    end

    return xi.combat.damage.scarletDeliriumMultiplierFromParams({
        hasEffect = hasEffect,
        power     = power,
    })
end

-- Handles Automaton attachment "Steam Jacket", which reduces damage from consecutive elemental damage.
xi.combat.damage.steamJacketMultiplier = function(target, magicalElement)
    -- Early return: Action isn't elemental (no local-var write).
    if magicalElement < xi.element.FIRE or magicalElement > xi.element.DARK then
        return 1
    end

    -- Early return: Target can't track an element (no local-var write).
    local steamJacketModifierValue = target:getMod(xi.mod.AUTO_STEAM_JACKET_REDUCTION)
    if steamJacketModifierValue <= 0 then
        return 1
    end

    -- Handle element tracking (host residual write).
    local trackedElement = target:getLocalVar(xi.combat.damage.steamJacketLocalVar)
    target:setLocalVar(xi.combat.damage.steamJacketLocalVar, magicalElement)

    return xi.combat.damage.steamJacketMultiplierFromParams({
        magicalElement = magicalElement,
        reductionMod   = steamJacketModifierValue,
        priorTracked   = trackedElement,
    })
end
