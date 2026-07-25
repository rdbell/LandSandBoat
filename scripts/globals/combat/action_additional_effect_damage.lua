-----------------------------------
-- Global file for additional effects (damage)
-- Pure injects dual-wired to OmegaXI internal/addeffectdamage (slice 6702 / 0946 / 6093).
-----------------------------------
require('scripts/globals/combat/damage_multipliers')
require('scripts/globals/combat/magic_hit_rate')
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.action = xi.combat.action or {}
-----------------------------------

xi.combat.action.addEffectDamageDefaultChance = 100
xi.combat.action.addEffectDamageDefaultLowestResist = 0.125
xi.combat.action.addEffectDamageClampMax = 99999

-- Element → subEffect animation defaults.
xi.combat.action.addEffectDamageDefaultsTable =
{
    [xi.element.NONE   ] = { xi.subEffect.LIGHT_DAMAGE     }, -- Like Excalibur.
    [xi.element.FIRE   ] = { xi.subEffect.FIRE_DAMAGE      },
    [xi.element.ICE    ] = { xi.subEffect.ICE_DAMAGE       },
    [xi.element.WIND   ] = { xi.subEffect.WIND_DAMAGE      },
    [xi.element.EARTH  ] = { xi.subEffect.EARTH_DAMAGE     },
    [xi.element.THUNDER] = { xi.subEffect.LIGHTNING_DAMAGE },
    [xi.element.WATER  ] = { xi.subEffect.WATER_DAMAGE     },
    [xi.element.LIGHT  ] = { xi.subEffect.LIGHT_DAMAGE     },
    [xi.element.DARK   ] = { xi.subEffect.DARKNESS_DAMAGE  },
}

-- En-spells override innate/weapon additional effects (shared with status path).
xi.combat.action.enspellEffects =
{
    xi.effect.ENFIRE,
    xi.effect.ENFIRE_II,
    xi.effect.ENBLIZZARD,
    xi.effect.ENBLIZZARD_II,
    xi.effect.ENAERO,
    xi.effect.ENAERO_II,
    xi.effect.ENSTONE,
    xi.effect.ENSTONE_II,
    xi.effect.ENTHUNDER,
    xi.effect.ENTHUNDER_II,
    xi.effect.ENWATER,
    xi.effect.ENWATER_II,
    xi.effect.ENLIGHT,
    xi.effect.ENDARK,
}

-----------------------------------
-- Pure injects
-----------------------------------

xi.combat.action.animationForMagicalElement = function(magicalElement)
    local row = xi.combat.action.addEffectDamageDefaultsTable[magicalElement or xi.element.NONE]
    if row then
        return row[1]
    end

    return 0
end

-- Pure validateParameters once fedData is injected (aeTarget deferred to host).
-- Numeric fields use Lua `or` falsy semantics (zero → default).
xi.combat.action.validateAddEffectDamageParams = function(fedData)
    fedData = fedData or {}
    local params = {}

    params.chance          = fedData.chance or xi.combat.action.addEffectDamageDefaultChance
    params.limitUndead     = fedData.limitUndead or false
    params.attackType      = fedData.attackType or xi.attackType.SPECIAL
    params.physicalElement = fedData.physicalElement or xi.damageType.NONE
    params.magicalElement  = fedData.magicalElement or xi.element.NONE
    params.basePower       = fedData.basePower or 0
    params.actorStat       = fedData.actorStat or 0
    params.targetStat      = fedData.targetStat or params.actorStat
    params.canMAB          = fedData.canMAB or false
    params.canResist       = fedData.canResist or false
    params.lowestResist    = fedData.lowestResist or xi.combat.action.addEffectDamageDefaultLowestResist
    params.canResistExtra  = fedData.canResistExtra or false
    params.drainHP         = fedData.drainHP or false
    params.drainMP         = fedData.drainMP or false
    params.drainTP         = fedData.drainTP or false
    params.overDrain       = fedData.overDrain or false
    params.animation       = fedData.animation or xi.combat.action.animationForMagicalElement(params.magicalElement)
    params.messageDamage   = fedData.messageDamage or xi.msg.basic.ADD_EFFECT_DMG
    params.messageHeal     = fedData.messageHeal or xi.msg.basic.ADD_EFFECT_HEAL

    return params
end

-- Pure hasEnspell once status presence for the enspell list is injected.
-- hasStatus is a map effectId → true, or a function(effectId) → bool.
xi.combat.action.hasEnspellFromParams = function(hasStatus)
    if not hasStatus then
        return false
    end

    local check = hasStatus
    if type(hasStatus) == 'table' then
        check = function(id)
            return hasStatus[id] == true
        end
    end

    for i = 1, #xi.combat.action.enspellEffects do
        if check(xi.combat.action.enspellEffects[i]) then
            return true
        end
    end

    return false
end

xi.combat.action.procMiss = function(roll, chance)
    return (roll or 0) > (chance or 0)
end

xi.combat.action.resistBelowFloor = function(resist, lowestResist)
    return (resist or 0) < (lowestResist or 0)
end

xi.combat.action.baseDamageFromParams = function(params)
    return (params.basePower or 0) + (params.actorStatMod or 0) - (params.targetStatMod or 0)
end

-- Pure floor-product once gates and multipliers are injected.
-- params: hasEnspell, procMiss, limitUndead, resistBelowFloor, base,
--   absorption, nullification, damageTypeSDT, physicalElementSDT, magicalElementSDT,
--   staff, affinity, dayWeather, magicDiff, resist, forcedResistTier
xi.combat.action.addEffectDamageProductFromParams = function(params)
    if
        params.hasEnspell or
        params.procMiss or
        params.limitUndead or
        params.resistBelowFloor
    then
        return 0
    end

    local damage = params.base or 0
    damage = math.floor(damage * (params.absorption or 1))
    damage = math.floor(damage * (params.nullification or 1))
    damage = math.floor(damage * (params.damageTypeSDT or 1))
    damage = math.floor(damage * (params.physicalElementSDT or 1))
    damage = math.floor(damage * (params.magicalElementSDT or 1))
    damage = math.floor(damage * (params.staff or 1))
    damage = math.floor(damage * (params.affinity or 1))
    damage = math.floor(damage * (params.dayWeather or 1))
    damage = math.floor(damage * (params.magicDiff or 1))
    damage = math.floor(damage * (params.resist or 1))
    damage = math.floor(damage * (params.forcedResistTier or 1))

    return damage
end

xi.combat.action.drainClamp = function(damage, resource, overDrain)
    if overDrain then
        return damage
    end

    return utils.clamp(damage, 0, resource or 0)
end

-----------------------------------
-- Entity host
-----------------------------------
xi.combat.action.executeAddEffectDamage = function(actor, target, fedData)
    local params = xi.combat.action.validateAddEffectDamageParams(fedData)
    params.aeTarget = (fedData and fedData.aeTarget) or target

    -- Early return: En-spell overrides innate/weapon additional effects.
    if xi.combat.action.hasEnspellFromParams(function(id)
        return actor:hasStatusEffect(id)
    end) then
        return 0, 0, 0
    end

    -- Early return: No proc.
    if xi.combat.action.procMiss(math.random(1, 100), params.chance) then
        return 0, 0, 0
    end

    -- Early return: Limit undead.
    if params.limitUndead and params.aeTarget:isUndead() then
        return 0, 0, 0
    end

    -- Additional variables.
    local isPhysical = params.attackType == xi.attackType.PHYSICAL or false
    local isMagical  = params.attackType == xi.attackType.MAGICAL or false
    local isRanged   = params.attackType == xi.attackType.RANGED or false
    local isBreath   = params.attackType == xi.attackType.BREATH or false

    local base = xi.combat.action.baseDamageFromParams({
        basePower      = params.basePower,
        actorStatMod   = actor:getMod(params.actorStat),
        targetStatMod  = params.aeTarget:getMod(params.targetStat),
    })

    local multiplierAbsorption         = xi.spells.damage.calculateAbsorption(params.aeTarget, params.magicalElement, isMagical)
    local multiplierNullification      = xi.spells.damage.calculateNullification(params.aeTarget, params.magicalElement, isMagical, isBreath)
    local multiplierDamageTypeSDT      = xi.combat.damage.calculateDamageAdjustment(params.aeTarget, isPhysical, isMagical, isRanged, isBreath)
    local multiplierPhysicalElementSDT = xi.combat.damage.physicalElementSDT(params.aeTarget, params.physicalElement)
    local multiplierMagicalElementSDT  = xi.combat.damage.magicalElementSDT(params.aeTarget, params.magicalElement)
    local multiplierElementalStaff     = xi.spells.damage.calculateElementalStaffBonus(actor, params.magicalElement)
    local multiplierElementalAffinity  = xi.spells.damage.calculateElementalAffinityBonus(actor, params.magicalElement)
    local multiplierDayWeather         = xi.spells.damage.calculateDayAndWeather(actor, params.magicalElement, false)
    local multiplierMagicDiff          = params.canMAB and xi.spells.damage.calculateMagicBonusDiff(actor, params.aeTarget, 0, 0, params.magicalElement, 0) or 1
    local multiplierResist             = params.canResist and xi.combat.magicHitRate.calculateResistRate(actor, params.aeTarget, 0, 0, xi.skillRank.A_PLUS, params.magicalElement, params.actorStat, 0, 0) or 1
    local multiplierForcedResistTier   = params.canResistExtra and xi.spells.damage.calculateAdditionalResistTier(actor, params.aeTarget, params.magicalElement) or 1

    -- Early return: Resist state is too low. Auto-fail.
    if xi.combat.action.resistBelowFloor(multiplierResist, params.lowestResist) then
        return 0, 0, 0
    end

    local damage = xi.combat.action.addEffectDamageProductFromParams({
        base               = base,
        absorption         = multiplierAbsorption,
        nullification      = multiplierNullification,
        damageTypeSDT      = multiplierDamageTypeSDT,
        physicalElementSDT = multiplierPhysicalElementSDT,
        magicalElementSDT  = multiplierMagicalElementSDT,
        staff              = multiplierElementalStaff,
        affinity           = multiplierElementalAffinity,
        dayWeather         = multiplierDayWeather,
        magicDiff          = multiplierMagicDiff,
        resist             = multiplierResist,
        forcedResistTier   = multiplierForcedResistTier,
    })

    -- Phalanx, One for all, Stoneskin.
    if damage > 0 then
        damage = utils.clamp(utils.handlePhalanx(params.aeTarget, damage), 0, xi.combat.action.addEffectDamageClampMax)
        damage = utils.clamp(utils.handleOneForAll(params.aeTarget, damage), 0, xi.combat.action.addEffectDamageClampMax)
        damage = utils.clamp(utils.handleStoneskin(params.aeTarget, damage), 0, xi.combat.action.addEffectDamageClampMax)
    end

    -- Drain HP, MP or TP
    if params.drainHP then
        damage               = xi.combat.action.drainClamp(damage, params.aeTarget:getHP(), params.overDrain)
        params.messageDamage = xi.msg.basic.ADD_EFFECT_HP_DRAIN
        actor:addHP(damage)
    end

    if params.drainMP then
        damage               = xi.combat.action.drainClamp(damage, params.aeTarget:getMP(), params.overDrain)
        params.messageDamage = xi.msg.basic.ADD_EFFECT_MP_DRAIN
        actor:addMP(damage)
    end

    if params.drainTP then
        damage               = xi.combat.action.drainClamp(damage, params.aeTarget:getTP(), params.overDrain)
        params.messageDamage = xi.msg.basic.ADD_EFFECT_TP_DRAIN
        actor:addTP(damage)
    end

    -- No damage, no proc.
    if damage == 0 then
        return 0, 0, 0
    end

    if damage < 0 then
        params.aeTarget:addHP(-damage) -- Heal target.
        return params.animation, params.messageHeal, -damage
    end

    local actionDamageType = params.physicalElement > 0 and params.physicalElement or xi.damageType.ELEMENTAL + params.magicalElement
    params.aeTarget:takeDamage(damage, actor, params.attackType, actionDamageType)

    return params.animation, params.messageDamage, damage
end
