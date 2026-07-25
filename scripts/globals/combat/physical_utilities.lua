-----------------------------------
-- Global, independent functions for physical calculations.
-- Includes:
-- fSTR, fSTR2, WSC, fTP, pDIF

-- For weapon skill:
-- Damage PER HIT = floor((D + fSTR + WSC) * fTP) * pDIF

-- Intended WS structure:
-- 1  - Calculate number of hits (max 8)
-- 1a - Calculate hits absorbed by blink and utsusemi, if aplicable.

-- 2  - Calculate first hit:
-- 2a - Calculate if first hit lands
-- 2b - Calculate if first hit crits
-- 2c - Calculate first hit DMG

-- 3  - Calculate, per hit, secondary hits, following the same structure as before, but simplified (no first-hit bonuses)

-- 4  - Add them all, and final operations/considerations.
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.physical = xi.combat.physical or {}
-----------------------------------
-----------------------------------
-- Pure WS elemental membership + fTP gear bonus (OmegaXI slice 6690)
-- Dual-wired to internal/ftpbonus.
-----------------------------------
-- [Skillchain type] = { Fire, Ice, Wind, Earth, Thunder, Water, Light, Dark }
xi.combat.physical.wsElementalProperties =
{
    [xi.skillchainType.NONE         ] = { 0, 0, 0, 0, 0, 0, 0, 0 }, -- Lv0 None
    [xi.skillchainType.LIQUEFACTION ] = { 1, 0, 0, 0, 0, 0, 0, 0 }, -- Lv1 Fire
    [xi.skillchainType.INDURATION   ] = { 0, 1, 0, 0, 0, 0, 0, 0 }, -- Lv1 Ice
    [xi.skillchainType.DETONATION   ] = { 0, 0, 1, 0, 0, 0, 0, 0 }, -- Lv1 Wind
    [xi.skillchainType.SCISSION     ] = { 0, 0, 0, 1, 0, 0, 0, 0 }, -- Lv1 Earth
    [xi.skillchainType.IMPACTION    ] = { 0, 0, 0, 0, 1, 0, 0, 0 }, -- Lv1 Thunder
    [xi.skillchainType.REVERBERATION] = { 0, 0, 0, 0, 0, 1, 0, 0 }, -- Lv1 Water
    [xi.skillchainType.TRANSFIXION  ] = { 0, 0, 0, 0, 0, 0, 1, 0 }, -- Lv1 Light
    [xi.skillchainType.COMPRESSION  ] = { 0, 0, 0, 0, 0, 0, 0, 1 }, -- Lv1 Dark
    [xi.skillchainType.FUSION       ] = { 1, 0, 0, 0, 0, 0, 1, 0 }, -- Lv2 Fire & Light
    [xi.skillchainType.DISTORTION   ] = { 0, 1, 0, 0, 0, 1, 0, 0 }, -- Lv2 Ice & Water
    [xi.skillchainType.FRAGMENTATION] = { 0, 0, 1, 0, 1, 0, 0, 0 }, -- Lv2 Wind & Thunder
    [xi.skillchainType.GRAVITATION  ] = { 0, 0, 0, 1, 0, 0, 0, 1 }, -- Lv2 Earth & Dark
    [xi.skillchainType.LIGHT        ] = { 1, 0, 1, 0, 1, 0, 1, 0 }, -- Lv3 Fire, Wind, Thunder, Light
    [xi.skillchainType.DARKNESS     ] = { 0, 1, 0, 1, 0, 1, 0, 1 }, -- Lv3 Ice, Earth, Water, Dark
    [xi.skillchainType.LIGHT_II     ] = { 1, 0, 1, 0, 1, 0, 1, 0 }, -- Lv4 Fire, Wind, Thunder, Light
    [xi.skillchainType.DARKNESS_II  ] = { 0, 1, 0, 1, 0, 1, 0, 1 }, -- Lv4 Ice, Earth, Water, Dark
}

xi.combat.physical.ftpDivisor = 256

-- Pure: skillchain type carries magic element (wsElementalProperties).
xi.combat.physical.hasWSElement = function(sc, el)
    if not el or el < xi.element.FIRE or el > xi.element.DARK then
        return false
    end

    local row = xi.combat.physical.wsElementalProperties[sc or 0]
    if not row then
        return false
    end

    return row[el] == 1
end

-- Pure: any of three WS skillchain props carries element.
xi.combat.physical.propsCarryElement = function(sc1, sc2, sc3, el)
    return xi.combat.physical.hasWSElement(sc1, el) or
        xi.combat.physical.hasWSElement(sc2, el) or
        xi.combat.physical.hasWSElement(sc3, el)
end

-- Pure calculateFTPBonus once actor type, SC props, day, and gear mods are injected.
-- params: isPC, scProp1/2/3, dayElement, elementFTPMods[FIRE..DARK], dayFTPBonus, anyFTPBonus
-- elementFTPMods entries are raw getMod amounts (divided by 256 inside).
xi.combat.physical.ftpBonusFromParams = function(params)
    if not params.isPC then
        return 0
    end

    local sc1 = params.scProp1 or xi.skillchainType.NONE
    local sc2 = params.scProp2 or xi.skillchainType.NONE
    local sc3 = params.scProp3 or xi.skillchainType.NONE

    if
        sc1 == xi.skillchainType.NONE and
        sc2 == xi.skillchainType.NONE and
        sc3 == xi.skillchainType.NONE
    then
        return 0
    end

    local fTPBonus = 0
    local mods = params.elementFTPMods or {}
    local dayElement = params.dayElement or 0
    local dayFTPBonus = params.dayFTPBonus or 0
    local divisor = xi.combat.physical.ftpDivisor

    for elementChecked = xi.element.FIRE, xi.element.DARK do
        if xi.combat.physical.propsCarryElement(sc1, sc2, sc3, elementChecked) then
            fTPBonus = fTPBonus + (mods[elementChecked] or 0) / divisor

            if dayElement == elementChecked then
                fTPBonus = fTPBonus + dayFTPBonus / divisor
            end
        end
    end

    fTPBonus = fTPBonus + (params.anyFTPBonus or 0) / divisor

    return fTPBonus
end

-- Table with pDIF caps per weapon/skill type.
xi.combat.physical.pDifWeaponCapTable =
{
    -- [Skill/weapon type used] = { pre-cRatio caps, pre-randomizer pDIF cap }, Values from: https://www.bg-wiki.com/ffxi/PDIF
    [xi.skill.NONE            ] = 3, -- Fallback, not intentionally used. Mobs are calculated in pdif functions.
    [xi.skill.HAND_TO_HAND    ] = 3.5,
    [xi.skill.DAGGER          ] = 3.25,
    [xi.skill.SWORD           ] = 3.25,
    [xi.skill.GREAT_SWORD     ] = 3.75,
    [xi.skill.AXE             ] = 3.25,
    [xi.skill.GREAT_AXE       ] = 3.75,
    [xi.skill.SCYTHE          ] = 4,
    [xi.skill.POLEARM         ] = 3.75,
    [xi.skill.KATANA          ] = 3.25,
    [xi.skill.GREAT_KATANA    ] = 3.5,
    [xi.skill.CLUB            ] = 3.25,
    [xi.skill.STAFF           ] = 3.75,
    [xi.skill.AUTOMATON_MELEE ] = 3,    -- Unknown value. Copy of value below.
    [xi.skill.AUTOMATON_RANGED] = 3,    -- Unknown value. Reference found in an old post: https://forum.square-enix.com/ffxi/archive/index.php/t-52778.html?s=d906df07788334a185a902b0a6ae6a99
    [xi.skill.AUTOMATON_MAGIC ] = 3,    -- Unknown value. Here for completion sake.
    [xi.skill.ARCHERY         ] = 3.25,
    [xi.skill.MARKSMANSHIP    ] = 3.5,
    [xi.skill.THROWING        ] = 3.25,
}

-- Pure weapon pDIF cap lookup (OmegaXI slice 6684 dual-wire of pDifWeaponCapTable).
-- Unknown skill types fall back to 3 (NONE entry / DefaultWeaponCap).
xi.combat.physical.defaultWeaponCap = 3

xi.combat.physical.weaponCap = function(skillType)
    local cap = xi.combat.physical.pDifWeaponCapTable[skillType]

    if cap == nil then
        return xi.combat.physical.defaultWeaponCap
    end

    return cap
end

-- Pure shield-size → base block rate (OmegaXI slice 6688 dual-wire).
xi.combat.physical.shieldSizeToBlockRateTable =
{
    [1] =  55, -- Buckler
    [2] =  40, -- Round
    [3] =  45, -- Kite
    [4] =  30, -- Tower
    [5] =  50, -- Aegis and Srivatsa
    [6] = 100, -- Ochain  https://www.bg-wiki.com/ffxi/Category:Shields
}

xi.combat.physical.blockRateMin = 5
xi.combat.physical.blockRateMax = 100
xi.combat.physical.blockSkillDeltaCoeff = 0.2325
xi.combat.physical.automatonBlockSkillDeltaCoeff = 0.215
xi.combat.physical.reprisalSkillScale = 1.15
xi.combat.physical.reprisalMultDefault = 1.5
xi.combat.physical.reprisalMultBonus = 3.0
xi.combat.physical.nonPCBlockAbsorbFraction = 0.5
xi.combat.physical.softMaxTrustLevel = 99

-----------------------------------
-- Pure: calculateAttackDamage inject halves (slice 6760 / internal/attack)
-- Matches production C++/Go path (additive DA/TA dmg mod quirk from 1577).
-- Host residual: consumeMana/souleater/addDamageFromMultipliers/restraint.
-----------------------------------
xi.combat.physical.mobH2HPenaltyPreToAU    = 0.425
xi.combat.physical.mobH2HPenaltyToAUOnward = 0.65
xi.combat.physical.mobKickPenalty          = 2 / 3

-- params: dex, sneakAtkDexMod
xi.combat.physical.sneakAttackDexBonusFromParams = function(params)
    params = params or {}
    local bonusPct = (params.sneakAtkDexMod or 0) / 100
    if bonusPct < 0 then
        bonusPct = 0
    end

    return (params.dex or 0) * (1 + bonusPct)
end

-- params: agi, trickAtkAgiMod
xi.combat.physical.trickAttackAgiBonusFromParams = function(params)
    params = params or {}
    local bonusPct = (params.trickAtkAgiMod or 0) / 100
    if bonusPct < 0 then
        bonusPct = 0
    end

    return (params.agi or 0) * (1 + bonusPct)
end

-- params: h2hSkill
xi.combat.physical.naturalH2hDamageFromParams = function(params)
    params = params or {}
    return math.floor((params.h2hSkill or 0) * 0.11) + 3
end

-- params: noH2HPenaltyMod, isPreToAURegion
xi.combat.physical.mobH2HPenaltyFromParams = function(params)
    params = params or {}
    if params.noH2HPenaltyMod then
        return 1
    end

    if params.isPreToAURegion then
        return xi.combat.physical.mobH2HPenaltyPreToAU
    end

    return xi.combat.physical.mobH2HPenaltyToAUOnward
end

-- params: baseDamagePlusBonus, isKick, kickDamageMod, fSTR, mobH2HPenalty
xi.combat.physical.assembleMobH2HPreRatioFromParams = function(params)
    params = params or {}
    local base = (params.baseDamagePlusBonus or 0)
    local fSTR = params.fSTR or 0
    local pen  = params.mobH2HPenalty or 1
    if params.isKick then
        return (base + (params.kickDamageMod or 0)) * pen * xi.combat.physical.mobKickPenalty + fSTR
    end

    return (base + fSTR) * pen
end

-- params: naturalH2h, kickDamageMod, bonus, fSTR
xi.combat.physical.assemblePlayerH2HKickPreRatioFromParams = function(params)
    params = params or {}
    local v = (params.naturalH2h or 0)
        + (params.kickDamageMod or 0)
        + (params.bonus or 0)
        + (params.fSTR or 0)
    if v < 0 then
        return 0
    end

    return v
end

-- params: weaponDmg, naturalH2h, bonus, fSTR
xi.combat.physical.assemblePlayerH2HPunchPreRatioFromParams = function(params)
    params = params or {}
    local v = (params.weaponDmg or 0)
        + (params.naturalH2h or 0)
        + (params.bonus or 0)
        + (params.fSTR or 0)
    if v < 0 then
        return 0
    end

    return v
end

-- params: weaponDmg, bonus, fSTR
xi.combat.physical.assembleWeaponPreRatioFromParams = function(params)
    params = params or {}
    local v = (params.weaponDmg or 0) + (params.bonus or 0) + (params.fSTR or 0)
    if v < 0 then
        return 0
    end

    return v
end

-- params: damage, damageRatio
xi.combat.physical.applyDamageRatioFromParams = function(params)
    params = params or {}
    local damage = params.damage or 0
    if damage <= 0 then
        return 0
    end

    return math.floor(damage * (params.damageRatio or 0))
end

-- Production additive DA/TA quirk (slice 1577): floor(damage * 1 + max(mod/100, 0))
-- params: damage, dmgMod
xi.combat.physical.applyDoubleTripleAttackDamageFromParams = function(params)
    params = params or {}
    local bonus = (params.dmgMod or 0) / 100
    if bonus < 0 then
        bonus = 0
    end

    return math.floor((params.damage or 0) * 1 + bonus)
end

-- params: damage, mult
xi.combat.physical.floorProductFromParams = function(params)
    params = params or {}
    local damage = params.damage or 0
    if damage <= 0 then
        return 0
    end

    return math.floor(damage * (params.mult or 0))
end

-- params: augmentsMod
xi.combat.physical.augmentDamageMultiplierFromParams = function(params)
    params = params or {}
    local bonus = (params.augmentsMod or 0) / 100
    if bonus < 0 then
        bonus = 0
    end

    return 1 + bonus
end

-- params: damage
xi.combat.physical.clampNonNegativeDamageFromParams = function(params)
    params = params or {}
    local damage = params.damage or 0
    if damage < 0 then
        return 0
    end

    return damage
end

-----------------------------------
-- Pure: Restraint WSD boost (slice 6768 / 2764)
-- Parity: internal/attack ShouldApplyRestraintBoost / ComputeRestraintWSDBoost /
-- ResolveRestraintWSDBoost; C++ attackhelpers same names.
-----------------------------------
xi.combat.physical.restraintMaxPower = 30

-- params: isFirstSwing, hasRestraint, powerLessThan30
xi.combat.physical.shouldApplyRestraintBoostFromParams = function(params)
    params = params or {}
    return not not params.isFirstSwing
        and not not params.hasRestraint
        and not not params.powerLessThan30
end

-- Pure math half once weaponDelayMs / power / subPower / enhances / jp inject.
-- params: weaponDelayMs, effectPower, effectSubPower, enhancesRestraint, jpBonus
-- returns: { boostAmount, newSubPower, applies = true }
xi.combat.physical.computeRestraintWSDBoostFromParams = function(params)
    params = params or {}
    local weaponDelayMs     = params.weaponDelayMs or 0
    local effectPower       = params.effectPower or 0
    local effectSubPower    = params.effectSubPower or 0
    local enhancesRestraint = params.enhancesRestraint or 0
    local jpBonus           = params.jpBonus or 0

    local boostPerRound = ((weaponDelayMs / 1000) * 60) / 385
    local remainder     = effectSubPower / 100

    boostPerRound = (boostPerRound
        * (1 + enhancesRestraint / 100)
        * (1 + jpBonus / 100)) + remainder

    -- (1 - (ceil - x)) * 100; integral boost stores 100 (LSB production quirk).
    remainder     = (1 - (math.ceil(boostPerRound) - boostPerRound)) * 100
    boostPerRound = math.floor(boostPerRound)

    if effectPower + boostPerRound > xi.combat.physical.restraintMaxPower then
        boostPerRound = xi.combat.physical.restraintMaxPower - effectPower
    end

    return {
        boostAmount = boostPerRound,
        newSubPower = math.floor(remainder),
        applies     = true,
    }
end

-- Combined gate + pure math for host writeback.
-- params: isFirstSwing, hasRestraint, effectPower, effectSubPower,
--   weaponDelayMs, enhancesRestraint, jpBonus
-- returns: { boostAmount, newSubPower, applies }
xi.combat.physical.resolveRestraintWSDBoostFromParams = function(params)
    params = params or {}
    local effectPower    = params.effectPower or 0
    local effectSubPower = params.effectSubPower or 0

    if not xi.combat.physical.shouldApplyRestraintBoostFromParams({
        isFirstSwing     = params.isFirstSwing,
        hasRestraint     = params.hasRestraint,
        powerLessThan30  = effectPower < xi.combat.physical.restraintMaxPower,
    })
    then
        return {
            boostAmount = 0,
            newSubPower = effectSubPower,
            applies     = false,
        }
    end

    return xi.combat.physical.computeRestraintWSDBoostFromParams({
        weaponDelayMs     = params.weaponDelayMs or 0,
        effectPower       = effectPower,
        effectSubPower    = effectSubPower,
        enhancesRestraint = params.enhancesRestraint or 0,
        jpBonus           = params.jpBonus or 0,
    })
end

--[[
  Pure product once injects are known (matches internal/attack.CalculateAttackDamage).
  params:
    isSneakAttack, isTrickAttack, dex, sneakAtkDexMod, agi, trickAtkAgiMod, consumeMana,
    isH2H, isMob, isKick, slot (MAIN/SUB/AMMO),
    weaponDmg, naturalH2h, kickDamageMod, fSTR, mobH2HPenalty, damageRatio,
    scarletMult, attackType (DOUBLE/TRIPLE/other), isPC, doubleAttackDmg, tripleAttackDmg,
    soulEater, useDamageMultipliers, damageAfterMultipliers,
    augmentsSA, hasSneakAttackEffect, augmentsTA, hasTrickAttackEffect
]]
xi.combat.physical.calculateAttackDamageFromParams = function(params)
    params = params or {}
    local bonus = 0
    if params.isSneakAttack then
        bonus = bonus + math.floor(xi.combat.physical.sneakAttackDexBonusFromParams({
            dex            = params.dex or 0,
            sneakAtkDexMod = params.sneakAtkDexMod or 0,
        }))
    end

    if params.isTrickAttack then
        bonus = bonus + math.floor(xi.combat.physical.trickAttackAgiBonusFromParams({
            agi            = params.agi or 0,
            trickAtkAgiMod = params.trickAtkAgiMod or 0,
        }))
    end

    bonus = bonus + (params.consumeMana or 0)

    local pre = 0
    local slot = params.slot
    if params.isH2H and params.isMob then
        pre = xi.combat.physical.assembleMobH2HPreRatioFromParams({
            baseDamagePlusBonus = (params.weaponDmg or 0) + bonus,
            isKick              = params.isKick,
            kickDamageMod       = params.kickDamageMod or 0,
            fSTR                = params.fSTR or 0,
            mobH2HPenalty       = params.mobH2HPenalty or 1,
        })
        -- cast pre to int then clamp then ratio (Go path)
        pre = math.floor(pre)
        if pre < 0 then
            pre = 0
        end
    elseif params.isH2H and params.isKick then
        pre = xi.combat.physical.assemblePlayerH2HKickPreRatioFromParams({
            naturalH2h    = params.naturalH2h or 0,
            kickDamageMod = params.kickDamageMod or 0,
            bonus         = bonus,
            fSTR          = params.fSTR or 0,
        })
    elseif params.isH2H then
        pre = xi.combat.physical.assemblePlayerH2HPunchPreRatioFromParams({
            weaponDmg  = params.weaponDmg or 0,
            naturalH2h = params.naturalH2h or 0,
            bonus      = bonus,
            fSTR       = params.fSTR or 0,
        })
    elseif slot == xi.slot.MAIN then
        pre = xi.combat.physical.assembleWeaponPreRatioFromParams({
            weaponDmg = params.weaponDmg or 0,
            bonus     = bonus,
            fSTR      = params.fSTR or 0,
        })
    elseif slot == xi.slot.SUB then
        pre = xi.combat.physical.assembleWeaponPreRatioFromParams({
            weaponDmg = params.weaponDmg or 0,
            bonus     = bonus,
            fSTR      = params.fSTR or 0,
        })
    elseif slot == xi.slot.AMMO then
        -- ammo ignores SA/TA bonus base
        pre = xi.combat.physical.assembleWeaponPreRatioFromParams({
            weaponDmg = params.weaponDmg or 0,
            bonus     = 0,
            fSTR      = params.fSTR or 0,
        })
    end

    local damage = xi.combat.physical.applyDamageRatioFromParams({
        damage      = pre,
        damageRatio = params.damageRatio or 0,
    })

    local scarlet = params.scarletMult or 1
    if scarlet ~= 1 and scarlet > 0 then
        damage = xi.combat.physical.floorProductFromParams({
            damage = damage,
            mult   = scarlet,
        })
    end

    local attackType = params.attackType
    if attackType == xi.physicalAttackType.DOUBLE and params.isPC then
        damage = xi.combat.physical.applyDoubleTripleAttackDamageFromParams({
            damage = damage,
            dmgMod = params.doubleAttackDmg or 0,
        })
    elseif attackType == xi.physicalAttackType.TRIPLE and params.isPC then
        damage = xi.combat.physical.applyDoubleTripleAttackDamageFromParams({
            damage = damage,
            dmgMod = params.tripleAttackDmg or 0,
        })
    end

    damage = damage + (params.soulEater or 0)

    if params.useDamageMultipliers then
        damage = params.damageAfterMultipliers or 0
    end

    if
        (params.augmentsSA or 0) > 0 and
        params.isSneakAttack and
        params.hasSneakAttackEffect
    then
        damage = xi.combat.physical.floorProductFromParams({
            damage = damage,
            mult   = xi.combat.physical.augmentDamageMultiplierFromParams({
                augmentsMod = params.augmentsSA or 0,
            }),
        })
    end

    if
        (params.augmentsTA or 0) > 0 and
        params.isTrickAttack and
        params.hasTrickAttackEffect
    then
        damage = xi.combat.physical.floorProductFromParams({
            damage = damage,
            mult   = xi.combat.physical.augmentDamageMultiplierFromParams({
                augmentsMod = params.augmentsTA or 0,
            }),
        })
    end

    return xi.combat.physical.clampNonNegativeDamageFromParams({ damage = damage })
end

-- WARNING: This function is used in src/map/attack.cpp "ProcessDamage" function.
-- If you update these parameters, update them there as well.
---@param actor CBaseEntity
---@param target CBaseEntity
---@param slot xi.slot
---@param physicalAttackType xi.physicalAttackType
---@param isH2H boolean
---@param isFirstSwing boolean
---@param isSneakAttack boolean
---@param isTrickAttack boolean
---@param damageRatio number
xi.combat.physical.calculateAttackDamage = function(actor, target, slot, physicalAttackType, isH2H, isFirstSwing, isSneakAttack, isTrickAttack, damageRatio)
    local naturalH2h = 0
    local weaponDmg  = 0
    local fSTR       = 0
    local mobH2HPenalty = 1
    local isKick = physicalAttackType == xi.physicalAttackType.KICK
    local isMob  = actor:isMob()

    if isH2H then
        naturalH2h = xi.combat.physical.naturalH2hDamageFromParams({
            h2hSkill = actor:getSkillLevel(xi.skill.HAND_TO_HAND),
        })
        if isMob then
            weaponDmg = actor:getWeaponDmg()
            fSTR = xi.combat.physical.calculateMeleeStatFactor(actor, target)
            mobH2HPenalty = xi.combat.physical.mobH2HPenaltyFromParams({
                noH2HPenaltyMod = actor:getMobMod(xi.mobMod.NO_H2H_PENALTY) ~= 0,
                isPreToAURegion = actor:getCurrentRegion() <= xi.region.LIMBUS,
            })
        else
            if not isKick then
                weaponDmg = actor:getWeaponDmg()
            end

            fSTR = xi.combat.physical.calculateMeleeStatFactor(actor, target)
        end
    elseif slot == xi.slot.MAIN then
        weaponDmg = actor:getWeaponDmg()
        fSTR = xi.combat.physical.calculateMeleeStatFactor(actor, target)
    elseif slot == xi.slot.SUB then
        weaponDmg = actor:getOffhandDmg()
        fSTR = xi.combat.physical.calculateMeleeStatFactor(actor, target)
    elseif slot == xi.slot.AMMO then
        weaponDmg = actor:getRangedDmg()
        fSTR = xi.combat.physical.calculateRangedStatFactor(actor, target)
    end

    -- Pure product through soul eater (before host damage multipliers / restraint).
    local damage = xi.combat.physical.calculateAttackDamageFromParams({
        isSneakAttack        = isSneakAttack,
        isTrickAttack        = isTrickAttack,
        dex                  = actor:getStat(xi.mod.DEX),
        sneakAtkDexMod       = actor:getMod(xi.mod.SNEAK_ATK_DEX),
        agi                  = actor:getStat(xi.mod.AGI),
        trickAtkAgiMod       = actor:getMod(xi.mod.TRICK_ATK_AGI),
        consumeMana          = xi.combat.damage.consumeManaAddition(actor),
        isH2H                = isH2H,
        isMob                = isMob,
        isKick               = isKick,
        slot                 = slot,
        weaponDmg            = weaponDmg,
        naturalH2h           = naturalH2h,
        kickDamageMod        = actor:getMod(xi.mod.KICK_DMG),
        fSTR                 = fSTR,
        mobH2HPenalty        = mobH2HPenalty,
        damageRatio          = damageRatio,
        scarletMult          = xi.combat.damage.scarletDeliriumMultiplier(actor),
        attackType           = physicalAttackType,
        isPC                 = actor:isPC(),
        doubleAttackDmg      = actor:getMod(xi.mod.DOUBLE_ATTACK_DMG),
        tripleAttackDmg      = actor:getMod(xi.mod.TRIPLE_ATTACK_DMG),
        soulEater            = xi.combat.damage.souleaterAddition(actor),
        useDamageMultipliers = false,
        augmentsSA           = 0,
        hasSneakAttackEffect = false,
        augmentsTA           = 0,
        hasTrickAttackEffect = false,
    })

    -- Host residual: CheckForDamageMultiplier
    damage = actor:addDamageFromMultipliers(damage, physicalAttackType, slot, isFirstSwing)

    -- Pure SA/TA augment floors after host multipliers.
    if
        actor:getMod(xi.mod.AUGMENTS_SA) > 0 and
        isSneakAttack and
        actor:hasStatusEffect(xi.effect.SNEAK_ATTACK)
    then
        damage = xi.combat.physical.floorProductFromParams({
            damage = damage,
            mult   = xi.combat.physical.augmentDamageMultiplierFromParams({
                augmentsMod = actor:getMod(xi.mod.AUGMENTS_SA),
            }),
        })
    end

    if
        actor:getMod(xi.mod.AUGMENTS_TA) > 0 and
        isTrickAttack and
        actor:hasStatusEffect(xi.effect.TRICK_ATTACK)
    then
        damage = xi.combat.physical.floorProductFromParams({
            damage = damage,
            mult   = xi.combat.physical.augmentDamageMultiplierFromParams({
                augmentsMod = actor:getMod(xi.mod.AUGMENTS_TA),
            }),
        })
    end

    damage = xi.combat.physical.clampNonNegativeDamageFromParams({ damage = damage })

    -- Apply Restraint Weaponskill Damage (pure dual-wire slice 6768 / 2764).
    -- Parity with C++ ProcessDamage / internal/attack ResolveRestraintWSDBoost.
    -- Intentional: Lua previously used (3*baseDelay/50)/385; production uses
    -- (weaponDelayMs/1000)*60/385. Host injects ms from base delay × 1000/60.
    if
        isFirstSwing and
        actor:hasStatusEffect(xi.effect.RESTRAINT)
    then
        local effect = actor:getStatusEffect(xi.effect.RESTRAINT)
        if effect then
            local jpBonus = 0
            if actor:isPC() then
                jpBonus = actor:getJobPointLevel(xi.jp.RESTRAINT_EFFECT) * 2
            end

            -- Convert FFXI delay units → ms (delay/60 seconds × 1000).
            local weaponDelayMs = math.floor((actor:getBaseDelay() or 0) * 1000 / 60)
            local plan = xi.combat.physical.resolveRestraintWSDBoostFromParams({
                isFirstSwing      = isFirstSwing,
                hasRestraint      = true,
                effectPower       = effect:getPower() or 0,
                effectSubPower    = effect:getSubPower() or 0,
                weaponDelayMs     = weaponDelayMs,
                enhancesRestraint = actor:getMod(xi.mod.ENHANCES_RESTRAINT) or 0,
                jpBonus           = jpBonus,
            })

            if plan.applies then
                effect:setPower((effect:getPower() or 0) + plan.boostAmount)
                effect:setSubPower(plan.newSubPower)
                -- C++ uses addModifier; prior Lua used setMod (parity with C++).
                actor:addMod(xi.mod.ALL_WSDMG_FIRST_HIT, plan.boostAmount)
            end
        end
    end

    -- TODO: add charutils::TrySkillUP call
    return damage
end

-----------------------------------
-- Pure fSTR / fSTR2 injects (OmegaXI slice 6687)
-- Dual-wired to internal/fstr.Melee / Ranged / MeleeMobPet / MeleePC / ...
-- BG wiki: https://www.bg-wiki.com/ffxi/FSTR
-----------------------------------

-- Shared player/trust piecewise raw addend before /4 (melee) or /2 (ranged).
xi.combat.physical.playerStatDiffRaw = function(statDiff)
    if statDiff >= 12 then
        return statDiff + 4
    elseif statDiff >= 6 then
        return statDiff + 6
    elseif statDiff >= 1 then
        return statDiff + 7
    elseif statDiff >= -2 then
        return statDiff + 8
    elseif statDiff >= -7 then
        return statDiff + 9
    elseif statDiff >= -15 then
        return statDiff + 10
    elseif statDiff >= -21 then
        return statDiff + 12
    end

    return statDiff + 13
end

-- Pure mob/pet melee ladder (unfloored /4).
xi.combat.physical.mobMeleeStatLadder = function(statDiff)
    if statDiff >= 36 then
        return (statDiff - 4) / 4
    elseif statDiff >= 26 then
        return (statDiff - 3) / 4
    elseif statDiff >= 17 then
        return (statDiff - 2) / 4
    elseif statDiff >= 4 then
        return (statDiff - 1) / 4
    elseif statDiff >= -8 then
        return statDiff / 4
    elseif statDiff >= -13 then
        return (statDiff + 1) / 4
    elseif statDiff >= -19 then
        return (statDiff + 3) / 4
    elseif statDiff >= -32 then
        return (statDiff + 4) / 4
    elseif statDiff >= -42 then
        return (statDiff + 5) / 4
    elseif statDiff >= -54 then
        return (statDiff + 6) / 4
    elseif statDiff >= -67 then
        return (statDiff + 7) / 4
    elseif statDiff >= -76 then
        return (statDiff + 8) / 4
    end

    return (statDiff + 9) / 4 -- <= -77
end

-- Pure mob/pet ranged ladder (unfloored /2); band thresholds differ from melee.
xi.combat.physical.mobRangedStatLadder = function(statDiff)
    if statDiff >= 36 then
        return (statDiff - 4) / 2
    elseif statDiff >= 26 then
        return (statDiff - 3) / 2
    elseif statDiff >= 15 then
        return (statDiff - 2) / 2
    elseif statDiff >= 4 then
        return (statDiff - 1) / 2
    elseif statDiff >= -8 then
        return statDiff / 2
    elseif statDiff >= -16 then
        return (statDiff + 1) / 2
    elseif statDiff >= -31 then
        return (statDiff + 1) / 2
    elseif statDiff >= -42 then
        return (statDiff + 3) / 2
    elseif statDiff >= -53 then
        return (statDiff + 3) / 2
    elseif statDiff >= -64 then
        return (statDiff + 5) / 2
    elseif statDiff >= -76 then
        return (statDiff + 6) / 2
    end

    return (statDiff + 7) / 2 -- <= -77
end

-- Pure mob/pet melee fSTR after early-return gate: floor ladder, level clamp.
xi.combat.physical.meleeStatFactorMobPet = function(statDiff, mainLvl)
    local fSTR = math.floor(xi.combat.physical.mobMeleeStatLadder(statDiff))

    return utils.clamp(fSTR, math.floor(mainLvl / 5) - 1, math.floor(mainLvl / 5) + 5)
end

-- Pure PC/trust melee fSTR: clamp dSTR to rank caps, raw addend, /4 clamp.
xi.combat.physical.meleeStatFactorPC = function(statDiff, weaponRank)
    local statLowerCap = (7 + weaponRank * 2) * -2
    local statUpperCap = (14 + weaponRank * 2) * 2

    statDiff = utils.clamp(statDiff, statLowerCap, statUpperCap)

    local raw = xi.combat.physical.playerStatDiffRaw(statDiff)
    local fSTRupperCap = weaponRank + 8
    local fSTRlowerCap = weaponRank * -1

    if weaponRank == 0 then
        fSTRlowerCap = -1
    end

    return utils.clamp(raw / 4, fSTRlowerCap, fSTRupperCap)
end

-- Pure mob/pet ranged fSTR2.
xi.combat.physical.rangedStatFactorMobPet = function(statDiff, mainLvl)
    local fSTR = math.floor(xi.combat.physical.mobRangedStatLadder(statDiff))
    local ml = mainLvl / 5

    return utils.clamp(fSTR, math.floor((ml - 1) * 2), math.floor((ml + 5) * 2))
end

-- Pure PC/trust ranged fSTR2.
xi.combat.physical.rangedStatFactorPC = function(statDiff, weaponRank)
    local statLowerCap = (7 + weaponRank * 2) * -2
    local statUpperCap = (14 + weaponRank * 2) * 2

    statDiff = utils.clamp(statDiff, statLowerCap, statUpperCap)

    local raw = xi.combat.physical.playerStatDiffRaw(statDiff)
    local fSTRupperCap = (weaponRank + 8) * 2
    local fSTRlowerCap = weaponRank * -2

    if weaponRank == 0 then
        fSTRlowerCap = -2
    elseif weaponRank == 1 then
        fSTRlowerCap = -3
    end

    return utils.clamp(raw / 2, fSTRlowerCap, fSTRupperCap)
end

-- Pure melee fSTR once actor kind / level / STR / VIT / weapon rank are injected.
-- isMob with mainLvl <= 1 → 1; isMob or isPet → mob/pet path; else PC/trust.
xi.combat.physical.meleeStatFactor = function(isMob, isPet, mainLvl, str, vit, weaponRank)
    if isMob and mainLvl <= 1 then
        return 1
    end

    local statDiff = str - vit

    if isMob or isPet then
        return xi.combat.physical.meleeStatFactorMobPet(statDiff, mainLvl)
    end

    return xi.combat.physical.meleeStatFactorPC(statDiff, weaponRank or 0)
end

-- Pure ranged fSTR2 with the same actor-kind rules as melee.
xi.combat.physical.rangedStatFactor = function(isMob, isPet, mainLvl, str, vit, weaponRank)
    if isMob and mainLvl <= 1 then
        return 1
    end

    local statDiff = str - vit

    if isMob or isPet then
        return xi.combat.physical.rangedStatFactorMobPet(statDiff, mainLvl)
    end

    return xi.combat.physical.rangedStatFactorPC(statDiff, weaponRank or 0)
end

-- Entity host: fSTR.
xi.combat.physical.calculateMeleeStatFactor = function(actor, target)
    return xi.combat.physical.meleeStatFactor(
        actor:isMob(),
        actor:isPet(),
        actor:getMainLvl(),
        actor:getStat(xi.mod.STR),
        target:getStat(xi.mod.VIT),
        actor:getWeaponDmgRank()
    )
end

-- Entity host: fSTR2.
xi.combat.physical.calculateRangedStatFactor = function(actor, target)
    return xi.combat.physical.rangedStatFactor(
        actor:isMob(),
        actor:isPet(),
        actor:getMainLvl(),
        actor:getStat(xi.mod.STR),
        target:getStat(xi.mod.VIT),
        actor:getRangedDmgRank()
    )
end

-- Weapon Skill Secondary Attribute Modifier: Function used to get stat addition to base damage.
-----------------------------------
-- Pure WSC injects (OmegaXI slice 6685)
-- Dual-wired to internal/wsc.Calculate / Term.
-- Distinct from blue magic WSC (no per-stat floor / no WS_*_BONUS there).
-----------------------------------

-- Pure single-term floor(stat * (multiplier + bonusPercent/100)).
xi.combat.physical.wscTerm = function(stat, multiplier, bonusPercent)
    return math.floor((stat or 0) * ((multiplier or 0) + (bonusPercent or 0) / 100))
end

-- Pure full WSC once stats, script multipliers, and WS_*_BONUS percents are known.
-- params.stats / .multipliers / .bonusPercents each have str,dex,vit,agi,int,mnd,chr
-- (nil fields sanitize to 0).
xi.combat.physical.wscFromParams = function(params)
    params = params or {}
    local stats = params.stats or {}
    local mults = params.multipliers or {}
    local bonus = params.bonusPercents or {}

    local term = xi.combat.physical.wscTerm

    return term(stats.str, mults.str, bonus.str) +
        term(stats.dex, mults.dex, bonus.dex) +
        term(stats.vit, mults.vit, bonus.vit) +
        term(stats.agi, mults.agi, bonus.agi) +
        term(stats.int, mults.int, bonus.int) +
        term(stats.mnd, mults.mnd, bonus.mnd) +
        term(stats.chr, mults.chr, bonus.chr)
end

-- Entity host: getStat / WS_*_BONUS injects → pure.
xi.combat.physical.calculateWSC = function(actor, wsSTRmod, wsDEXmod, wsVITmod, wsAGImod, wsINTmod, wsMNDmod, wsCHRmod)
    return xi.combat.physical.wscFromParams({
        stats = {
            str = actor:getStat(xi.mod.STR),
            dex = actor:getStat(xi.mod.DEX),
            vit = actor:getStat(xi.mod.VIT),
            agi = actor:getStat(xi.mod.AGI),
            int = actor:getStat(xi.mod.INT),
            mnd = actor:getStat(xi.mod.MND),
            chr = actor:getStat(xi.mod.CHR),
        },
        multipliers = {
            str = wsSTRmod or 0,
            dex = wsDEXmod or 0,
            vit = wsVITmod or 0,
            agi = wsAGImod or 0,
            int = wsINTmod or 0,
            mnd = wsMNDmod or 0,
            chr = wsCHRmod or 0,
        },
        bonusPercents = {
            str = actor:getMod(xi.mod.WS_STR_BONUS),
            dex = actor:getMod(xi.mod.WS_DEX_BONUS),
            vit = actor:getMod(xi.mod.WS_VIT_BONUS),
            agi = actor:getMod(xi.mod.WS_AGI_BONUS),
            int = actor:getMod(xi.mod.WS_INT_BONUS),
            mnd = actor:getMod(xi.mod.WS_MND_BONUS),
            chr = actor:getMod(xi.mod.WS_CHR_BONUS),
        },
    })
end

-- TP factor equation. Used to determine TP modifer across all cases of 'X varies with TP'
-- Pure TP-segment factor (OmegaXI slice 6684 dual-wire / 0840).
-- table[1]/[2]/[3] = values at 1000 / 2000 / 3000 TP.
xi.combat.physical.calculateTPfactor = function(actorTP, tpModifierTable)
    if not tpModifierTable then
        return 0
    end

    local tpFactor = tpModifierTable[1] -- Assume this will be used for monstrosity fixed TP moved someday.

    if actorTP >= 2000 then
        tpFactor = tpModifierTable[2] + (actorTP - 2000) * (tpModifierTable[3] - tpModifierTable[2]) / 1000
    elseif actorTP >= 1000 then
        tpFactor = tpModifierTable[1] + (actorTP - 1000) * (tpModifierTable[2] - tpModifierTable[1]) / 1000
    end

    return tpFactor
end

-- Entity host: inject actor type / SC props / day / gear mods → pure.
xi.combat.physical.calculateFTPBonus = function(actor)
    -- Gear bonuses only come from gear (PC).
    if actor:getObjType() ~= xi.objType.PC then
        return xi.combat.physical.ftpBonusFromParams({ isPC = false })
    end

    local scProp1, scProp2, scProp3 = actor:getWSSkillchainProp()
    local elementFTPMods = {}

    for elementChecked = xi.element.FIRE, xi.element.DARK do
        elementFTPMods[elementChecked] = actor:getMod(xi.data.element.getElementalFTPModifier(elementChecked))
    end

    return xi.combat.physical.ftpBonusFromParams({
        isPC           = true,
        scProp1        = scProp1,
        scProp2        = scProp2,
        scProp3        = scProp3,
        dayElement     = VanadielDayElement(),
        elementFTPMods = elementFTPMods,
        dayFTPBonus    = actor:getMod(xi.mod.DAY_FTP_BONUS),
        anyFTPBonus    = actor:getMod(xi.mod.ANY_FTP_BONUS),
    })
end

---@param wRatio number
---@param pDifFinalCap number
-- Pure PC wRatio → pDIF lower/upper caps (OmegaXI slice 6684 dual-wire / 0840).
-----------------------------------
-- Pure: cRatio level-correction factor (slice 6757 / internal/pdif.LevelDifFactor)
-- Melee slope 3/64; ranged 3/128. Non-PC clamps negative to 0; PC clamps positive to 0.
-- params: actorLevel, targetLevel, applyLevelCorrection, actorIsPC, ranged
-----------------------------------
xi.combat.physical.meleeLevelCorrectionPerLevel  = 3 / 64
xi.combat.physical.rangedLevelCorrectionPerLevel = 3 / 128
xi.combat.physical.rangedCRatioMin               = 0
xi.combat.physical.rangedCRatioMax               = 10

xi.combat.physical.levelDifFactorFromParams = function(params)
    params = params or {}
    if not params.applyLevelCorrection then
        return 0
    end

    local slope = xi.combat.physical.meleeLevelCorrectionPerLevel
    if params.ranged then
        slope = xi.combat.physical.rangedLevelCorrectionPerLevel
    end

    local factor = ((params.actorLevel or 0) - (params.targetLevel or 0)) * slope

    -- Only players suffer from negative level difference.
    if not params.actorIsPC and factor < 0 then
        return 0
    end

    -- Players do not get positive level correction, only monsters
    if params.actorIsPC and factor > 0 then
        return 0
    end

    return factor
end

-- Pure: clamp ranged base ratio to [0, 10]
xi.combat.physical.clampRangedCRatioFromParams = function(params)
    params = params or {}
    local baseRatio = params.baseRatio or 0
    if baseRatio < xi.combat.physical.rangedCRatioMin then
        return xi.combat.physical.rangedCRatioMin
    end

    if baseRatio > xi.combat.physical.rangedCRatioMax then
        return xi.combat.physical.rangedCRatioMax
    end

    return baseRatio
end

-- Pure: ranged pDIF cap bands from clamped cRatio (before level correction)
-- returns: lowerCap, upperCap
xi.combat.physical.rangedCRatioCapsFromParams = function(params)
    params = params or {}
    local cRatio       = params.cRatio or 0
    local pDifFinalCap = params.pDifFinalCap or 0

    if cRatio < 0.9 then
        return cRatio, cRatio * 10 / 9
    elseif cRatio < 1.1 then
        return 1, 1
    end

    return math.min(cRatio * 20 / 19 - 3 / 19, pDifFinalCap), math.min(cRatio, pDifFinalCap)
end

-- Pure: add levelDifFactor to both caps
xi.combat.physical.applyLevelDifToCapsFromParams = function(params)
    params = params or {}
    local factor = params.levelDifFactor or 0
    return (params.lowerCap or 0) + factor, (params.upperCap or 0) + factor
end

-----------------------------------
-- Pure: effective defense, final pDIF caps, crit damage mult (slice 6758)
-- Parity: internal/wsformula.EffectiveDefense; internal/pdif FinalCap* / CritDamageMult
-----------------------------------
-- params: defense, ignoresDefense, ignoreFraction (tpFactor when ignoring)
xi.combat.physical.effectiveDefenseFromParams = function(params)
    params = params or {}
    local def = params.defense or 0
    if def < 1 then
        def = 1
    end

    if not params.ignoresDefense then
        return def
    end

    return math.max(1, math.floor(def * (1 - (params.ignoreFraction or 0))))
end

-- params: weaponCap, damageLimit, damageLimitP, meleeCritBonus
xi.combat.physical.finalCapPCFromParams = function(params)
    params = params or {}
    local cap = ((params.weaponCap or 0) + (params.damageLimit or 0) / 100)
        * (1 + (params.damageLimitP or 0) / 100)
    if params.meleeCritBonus then
        cap = cap + 1
    end

    return cap
end

-- params: applyLevelCorrection, isCritical, damageLimit, damageLimitP
xi.combat.physical.finalCapMeleeOthersFromParams = function(params)
    params = params or {}
    local base = 4
    if params.applyLevelCorrection then
        base = 2
    end

    local critBonus = 0
    if params.applyLevelCorrection and params.isCritical then
        critBonus = 1
    end

    return (base + (params.damageLimit or 0) / 100)
        * (1 + (params.damageLimitP or 0) / 100)
        + critBonus
end

-- params: applyLevelCorrection, damageLimit, damageLimitP
xi.combat.physical.finalCapRangedOthersFromParams = function(params)
    params = params or {}
    local base = 4
    if params.applyLevelCorrection then
        base = 3
    end

    return (base + (params.damageLimit or 0) / 100)
        * (1 + (params.damageLimitP or 0) / 100)
end

-- params: critDmgIncrease, critDefBonus
-- returns multiplier (100 + clamp(increase - def, 0, 100)) / 100
xi.combat.physical.critDamageMultFromParams = function(params)
    params = params or {}
    local bonus = utils.clamp(
        (params.critDmgIncrease or 0) - (params.critDefBonus or 0),
        0,
        100
    )
    return (100 + bonus) / 100
end

-----------------------------------
-- Pure: Building Flourish attack bonus + actor attack product (slice 6759)
-- Parity: internal/pdif BuildingFlourishBonus / MeleeActorAttack /
-- RangedActorAttack / BaseRatio / MeleeWRatio / MeleeRandomFactor
-----------------------------------
xi.combat.physical.buildingFlourishMinPower   = 2
xi.combat.physical.buildingFlourishBaseBonus  = 1.25
xi.combat.physical.buildingFlourishMeritStep  = 0.01

-- params: hasEffect, power, meritCount
xi.combat.physical.buildingFlourishBonusFromParams = function(params)
    params = params or {}
    if not params.hasEffect or (params.power or 0) < xi.combat.physical.buildingFlourishMinPower then
        return 1
    end

    return xi.combat.physical.buildingFlourishBaseBonus
        + xi.combat.physical.buildingFlourishMeritStep * (params.meritCount or 0)
end

-- params: att, wsAttackMod, flourishBonus
xi.combat.physical.meleeActorAttackFromParams = function(params)
    params = params or {}
    return math.max(1, math.floor(
        (params.att or 0) * (params.wsAttackMod or 0) * (params.flourishBonus or 0)
    ))
end

-- params: ratt, bonusRangedAttack, distancePenalty, wsAttackMod, flourishBonus
xi.combat.physical.rangedActorAttackFromParams = function(params)
    params = params or {}
    return math.max(1, math.floor(
        ((params.ratt or 0) + (params.bonusRangedAttack or 0) - (params.distancePenalty or 0))
            * (params.wsAttackMod or 0)
            * (params.flourishBonus or 0)
    ))
end

-- params: actorAttack, targetDefense
xi.combat.physical.baseRatioFromParams = function(params)
    params = params or {}
    local def = params.targetDefense or 0
    if def == 0 then
        return 0
    end

    return (params.actorAttack or 0) / def
end

-- params: baseRatio, isCritical
xi.combat.physical.meleeWRatioFromParams = function(params)
    params = params or {}
    local base = params.baseRatio or 0
    if params.isCritical then
        return base + 1
    end

    return base
end

-- params: step (0..5)
xi.combat.physical.meleeRandomFactorFromParams = function(params)
    params = params or {}
    local step = params.step or 0
    if step < 0 then
        step = 0
    elseif step > 5 then
        step = 5
    end

    return 1 + step * 0.01
end

-----------------------------------
-- Pure: full MeleePDIF / RangedPDIF products (slice 6762 / 1572)
-- Parity: internal/pdif MeleePDIF / RangedPDIF / MeleeBounds / RangedBounds
-----------------------------------
xi.combat.physical.rangedCritMult = 1.25

-- Pure melee pDIF bounds after level correction for host RNG sampling.
-- params: actorAttack, targetDefense, isCritical, applyLevelCorrection,
--   actorIsPC, actorLevel, targetLevel, weaponCap, damageLimit, damageLimitP,
--   spikeRoll (1..10000), upperMaxCoin (0 or 1)
-- returns: lower, upper, spiked, spikePdif
xi.combat.physical.meleePDIFBoundsFromParams = function(params)
    params = params or {}
    local def = params.targetDefense or 0
    if def < 1 then
        def = 1
    end

    local baseRatio = xi.combat.physical.baseRatioFromParams({
        actorAttack   = params.actorAttack or 0,
        targetDefense = def,
    })
    local levelDif = xi.combat.physical.levelDifFactorFromParams({
        actorLevel           = params.actorLevel or 0,
        targetLevel          = params.targetLevel or 0,
        applyLevelCorrection = params.applyLevelCorrection,
        actorIsPC            = params.actorIsPC,
        ranged               = false,
    })
    local wRatio = xi.combat.physical.meleeWRatioFromParams({
        baseRatio  = baseRatio,
        isCritical = params.isCritical,
    })

    local pDifFinalCap = 0
    if params.actorIsPC then
        pDifFinalCap = xi.combat.physical.finalCapPCFromParams({
            weaponCap      = params.weaponCap or 0,
            damageLimit    = params.damageLimit or 0,
            damageLimitP   = params.damageLimitP or 0,
            meleeCritBonus = params.isCritical,
        })
    else
        pDifFinalCap = xi.combat.physical.finalCapMeleeOthersFromParams({
            applyLevelCorrection = params.applyLevelCorrection,
            isCritical           = params.isCritical,
            damageLimit          = params.damageLimit or 0,
            damageLimitP         = params.damageLimitP or 0,
        })
    end

    local sRatio = xi.combat.physical.spikeRatio(params.actorIsPC, wRatio)
    if ((params.spikeRoll or 0) / 10000) <= sRatio then
        return 0, 0, true, 1.0
    end

    local lowerCap, upperCap
    if params.actorIsPC then
        lowerCap, upperCap = xi.combat.physical.wRatioCapPC(wRatio, pDifFinalCap)
    else
        lowerCap, upperCap = xi.combat.physical.wRatioCapOthers(wRatio, pDifFinalCap)
    end

    local upperMax = 0
    if (params.upperMaxCoin or 0) == 0 then
        upperMax = 0.5
    end

    local upperBound = math.max(upperCap + levelDif, upperMax)
    local lowerBound = math.max(lowerCap + levelDif, 0)
    return lowerBound, upperBound, false, 0
end

-- Pure calculateMeleePDIF once attack/defense/level/cap injects and RNG rolls.
-- params: actorAttack, targetDefense, isCritical, applyLevelCorrection,
--   actorIsPC, actorLevel, targetLevel, weaponCap, damageLimit, damageLimitP,
--   critDmgIncrease, critDefBonus, spikeRoll, upperMaxCoin, ratioRoll,
--   ratioRollValid, meleeRandStep
xi.combat.physical.meleePDIFFromParams = function(params)
    params = params or {}
    local lower, upper, spiked, spikePdif = xi.combat.physical.meleePDIFBoundsFromParams(params)
    if spiked then
        return spikePdif
    end

    if upper == 0 then
        return 0
    end

    local pDif
    if params.ratioRollValid then
        pDif = (params.ratioRoll or 0) / 1000
    else
        pDif = (lower + upper) / 2
    end

    pDif = pDif * xi.combat.physical.meleeRandomFactorFromParams({
        step = params.meleeRandStep or 0,
    })

    if params.isCritical then
        pDif = pDif * xi.combat.physical.critDamageMultFromParams({
            critDmgIncrease = params.critDmgIncrease or 0,
            critDefBonus    = params.critDefBonus or 0,
        })
    end

    return pDif
end

-- Pure ranged pDIF bounds after level correction for host RNG sampling.
-- params: actorAttack, targetDefense, applyLevelCorrection, actorIsPC,
--   actorLevel, targetLevel, weaponCap, damageLimit, damageLimitP
-- returns: lower, upper
xi.combat.physical.rangedPDIFBoundsFromParams = function(params)
    params = params or {}
    local def = params.targetDefense or 0
    if def < 1 then
        def = 1
    end

    local baseRatio = xi.combat.physical.baseRatioFromParams({
        actorAttack   = params.actorAttack or 0,
        targetDefense = def,
    })
    local levelDif = xi.combat.physical.levelDifFactorFromParams({
        actorLevel           = params.actorLevel or 0,
        targetLevel          = params.targetLevel or 0,
        applyLevelCorrection = params.applyLevelCorrection,
        actorIsPC            = params.actorIsPC,
        ranged               = true,
    })
    local cRatio = xi.combat.physical.clampRangedCRatioFromParams({
        baseRatio = baseRatio,
    })

    local pDifFinalCap = 0
    if params.actorIsPC then
        pDifFinalCap = xi.combat.physical.finalCapPCFromParams({
            weaponCap      = params.weaponCap or 0,
            damageLimit    = params.damageLimit or 0,
            damageLimitP   = params.damageLimitP or 0,
            meleeCritBonus = false,
        })
    else
        pDifFinalCap = xi.combat.physical.finalCapRangedOthersFromParams({
            applyLevelCorrection = params.applyLevelCorrection,
            damageLimit          = params.damageLimit or 0,
            damageLimitP         = params.damageLimitP or 0,
        })
    end

    local lowerCap, upperCap = xi.combat.physical.rangedCRatioCapsFromParams({
        cRatio       = cRatio,
        pDifFinalCap = pDifFinalCap,
    })
    return xi.combat.physical.applyLevelDifToCapsFromParams({
        lowerCap       = lowerCap,
        upperCap       = upperCap,
        levelDifFactor = levelDif,
    })
end

-- Pure calculateRangedPDIF once attack/defense/level/cap injects and RNG rolls.
-- params: actorAttack, targetDefense, isCritical, applyLevelCorrection,
--   actorIsPC, actorLevel, targetLevel, weaponCap, damageLimit, damageLimitP,
--   critDmgIncrease, critDefBonus, ratioRoll, ratioRollValid
xi.combat.physical.rangedPDIFFromParams = function(params)
    params = params or {}
    local lower, upper = xi.combat.physical.rangedPDIFBoundsFromParams(params)

    local pDif
    if params.ratioRollValid then
        pDif = (params.ratioRoll or 0) / 1000
    else
        pDif = (lower + upper) / 2
    end

    if pDif < 0 then
        pDif = 0
    end

    if params.isCritical then
        pDif = pDif * xi.combat.physical.rangedCritMult
        pDif = pDif * xi.combat.physical.critDamageMultFromParams({
            critDmgIncrease = params.critDmgIncrease or 0,
            critDefBonus    = params.critDefBonus or 0,
        })
    end

    return pDif
end

xi.combat.physical.wRatioCapPC = function(wRatio, pDifFinalCap)
    local pDifUpperCap = 0
    local pDifLowerCap = 0

    -- pDIF upper cap.
    if wRatio < 0.5 then
        pDifUpperCap = wRatio + 0.5
    elseif wRatio < 0.7 then
        pDifUpperCap = 1
    elseif wRatio < 1.2 then
        pDifUpperCap = wRatio + 0.3
    elseif wRatio < 1.5 then
        pDifUpperCap = wRatio + wRatio * 0.25
    else
        pDifUpperCap = math.min(wRatio + 0.375, pDifFinalCap)
    end

    -- pDIF lower cap.
    if wRatio < 0.38 then
        pDifLowerCap = 0
    elseif wRatio < 1.25 then
        pDifLowerCap = wRatio * 1176 / 1024 - 448 / 1024
    elseif wRatio < 1.51 then
        pDifLowerCap = 1
    elseif wRatio < 2.44 then
        pDifLowerCap = wRatio * 1176 / 1024 - 775 / 1024
    else
        pDifLowerCap = math.min(wRatio - 0.375, pDifFinalCap)
    end

    return pDifLowerCap, pDifUpperCap
end

-- Pure non-PC wRatio → pDIF lower/upper caps (OmegaXI slice 6684 dual-wire / 0840).
-- see https://www.ffxiah.com/forum/topic/58479/monster-pdif-curves-and-other-info/
---@param wRatio number
---@param pDifFinalCap number
xi.combat.physical.wRatioCapOthers = function(wRatio, pDifFinalCap)
    local pDifUpperCap = 0
    local pDifLowerCap = 0

    -- see https://www.ffxiah.com/forum/topic/58479/monster-pdif-curves-and-other-info/
    -- pDIF upper cap.
    if wRatio < 0.55 then
        pDifUpperCap = 0.6 + 760 / 1024 * wRatio
    elseif wRatio <= 0.8 then
        pDifUpperCap = 1
    elseif wRatio < 1.2 then
        pDifUpperCap = 1 + 1127 / 1024 * (wRatio - 0.8)
    elseif wRatio < 1.5 then
        pDifUpperCap = 1474 / 1024 + 1105 / 1024 * (wRatio - 1235 / 1024)
    else
        pDifUpperCap = math.min(1803 / 1024 + 1070 / 1024 * (wRatio - 1.5), pDifFinalCap)
    end

    -- pDIF lower cap.
    if wRatio <= 0.4 then
        pDifLowerCap = 0.25
    elseif wRatio < 1.35 then
        pDifLowerCap = 0.25 + (827 / 1024) * (wRatio - 0.4)
    elseif wRatio <= 1.60 then
        pDifLowerCap = 1
    else
        pDifLowerCap = math.min(1 + (1120 / 1024) * (wRatio - 1.59), pDifFinalCap)
    end

    return pDifLowerCap, pDifUpperCap
end

---@param isPC boolean
---@param wRatio number
---@return number
-- Pure pDIF "spike" / qRatio mass from wRatio (OmegaXI slice 6677).
-- Dual-wired to internal/pdif.SpikeRatio / internal/pdifspike.
--
-- PC arm: https://www.bg-wiki.com/ffxi/PDIF#Average_Melee_pDIF(qRatio)
-- Non-PC: https://www.ffxiah.com/forum/topic/58479/monster-pdif-curves-and-other-info/#3751498
xi.combat.physical.spikeRatio = function(isPC, wRatio)
    if isPC then
        -- 0.5 and 1.5 are 0% chance (strict inequalities).
        if wRatio > 0.5 and wRatio < 1.5 then
            local sRatio = (0.5 - math.abs(wRatio - 1)) * 1.2

            return utils.clamp(sRatio, 0, 1 / 3) -- 1/3 (one-third), not 0.33
        end
    else
        local sRatio = 0

        if wRatio > 0.0 and wRatio < 0.75 then
            sRatio = -5 / 9 + (10 / 9) * wRatio
        elseif wRatio <= 1.3 then
            sRatio = 0.3
        else
            sRatio = 5 / 3 - (270 / 256) * wRatio
        end

        return utils.clamp(sRatio, 0, 0.3)
    end

    return 0
end

-- WARNING: This function is used in src/utils/battleutils.cpp "GetDamageRatio" function.
-- If you update this parameters, update them there aswell.
---@param actor CBaseEntity
---@param target CBaseEntity
---@param weaponType xi.skill
---@param wsAttackMod number
---@param isCritical boolean
---@param applyLevelCorrection boolean
---@param tpIgnoresDefense boolean
---@param tpFactor number
---@param isWeaponskill boolean
---@param weaponSlot xi.slot
---@param isCannonball boolean
-- Host residual: entity ATT/DEF/flourish/attuner/cannonball/level/cap mods + RNG.
-- Pure product: meleePDIFFromParams (slice 6762 / 1572).
xi.combat.physical.calculateMeleePDIF = function(actor, target, weaponType, wsAttackMod, isCritical, applyLevelCorrection, tpIgnoresDefense, tpFactor, isWeaponskill, weaponSlot, isCannonball)
    ----------------------------------------
    -- Step 1: Attack / Defense injects
    ----------------------------------------
    local targetDefense = math.max(1, target:getStat(xi.mod.DEF))
    local flourishBonus = 1

    -- Actor Weaponskill Specific Attack modifiers.
    if isWeaponskill then
        local flourishEffect = actor:getStatusEffect(xi.effect.BUILDING_FLOURISH)
        flourishBonus = xi.combat.physical.buildingFlourishBonusFromParams({
            hasEffect  = flourishEffect ~= nil,
            power      = flourishEffect and flourishEffect:getPower() or 0,
            meritCount = flourishEffect and flourishEffect:getSubPower() or 0,
        })
    end

    -- TODO: it is unknown if ws attack mod and flourish bonus are additive or multiplicative
    -- TODO: do flourish and attack mods come before or after food?
    local actorAttack = xi.combat.physical.meleeActorAttackFromParams({
        att            = actor:getStat(xi.mod.ATT, weaponSlot),
        wsAttackMod    = wsAttackMod,
        flourishBonus  = flourishBonus,
    })

    -- handle attuner
    -- note: isAutomaton is checked inside xi.automaton.handleAttuner and could be removed
    if actor:isAutomaton() then
        local defIgnore = xi.automaton.handleAttuner(actor, target)

        tpFactor = tpFactor + defIgnore

        if tpFactor > 0 then
            tpIgnoresDefense = true
        end
    end

    -- Target Defense Modifiers.
    targetDefense = xi.combat.physical.effectiveDefenseFromParams({
        defense         = targetDefense,
        ignoresDefense  = tpIgnoresDefense,
        ignoreFraction  = tpFactor,
    })

    if isCannonball then
        actorAttack = actor:getStat(xi.mod.DEF)
    end

    local inject =
    {
        actorAttack          = actorAttack,
        targetDefense        = targetDefense,
        isCritical           = isCritical,
        applyLevelCorrection = applyLevelCorrection,
        actorIsPC            = actor:isPC(),
        actorLevel           = actor:getMainLvl(),
        targetLevel          = target:getMainLvl(),
        weaponCap            = xi.combat.physical.weaponCap(weaponType),
        damageLimit          = actor:getMod(xi.mod.DAMAGE_LIMIT),
        damageLimitP         = actor:getMod(xi.mod.DAMAGE_LIMITP),
        critDmgIncrease      = actor:getMod(xi.mod.CRIT_DMG_INCREASE),
        critDefBonus         = target:getMod(xi.mod.CRIT_DEF_BONUS),
        spikeRoll            = math.random(1, 10000),
        upperMaxCoin         = math.random(0, 1),
        meleeRandStep        = math.random(0, 5),
    }

    local lower, upper, spiked, spikePdif = xi.combat.physical.meleePDIFBoundsFromParams(inject)
    if spiked then
        return spikePdif
    end

    if upper == 0 then
        return 0
    end

    inject.ratioRollValid = true
    inject.ratioRoll = math.random(lower * 1000, upper * 1000)
    return xi.combat.physical.meleePDIFFromParams(inject)
end

---@param actor CBaseEntity
---@param target CBaseEntity
---@param weaponType xi.skill
---@param wsAttackMod number
---@param isCritical boolean
---@param applyLevelCorrection boolean
---@param tpIgnoresDefense boolean
---@param tpFactor number
---@param isWeaponskill boolean
---@param bonusRangedAttack integer
-- Host residual: entity RATT/DEF/distance/flourish/level/cap mods + RNG.
-- Pure product: rangedPDIFFromParams (slice 6762 / 1572).
xi.combat.physical.calculateRangedPDIF = function(actor, target, weaponType, wsAttackMod, isCritical, applyLevelCorrection, tpIgnoresDefense, tpFactor, isWeaponskill, bonusRangedAttack)
    ----------------------------------------
    -- Step 1: Attack / Defense injects
    ----------------------------------------
    local targetDefense   = math.max(1, target:getStat(xi.mod.DEF))
    local flourishBonus   = 1
    local distancePenalty = 0

    if not actor:isMob() then
        distancePenalty = xi.combat.ranged.attackDistancePenalty(actor, target)
    end

    -- Actor Weaponskill Specific Attack modifiers.
    -- TODO: verify this actually works on ranged WS
    if isWeaponskill then
        local flourishEffect = actor:getStatusEffect(xi.effect.BUILDING_FLOURISH)
        flourishBonus = xi.combat.physical.buildingFlourishBonusFromParams({
            hasEffect  = flourishEffect ~= nil,
            power      = flourishEffect and flourishEffect:getPower() or 0,
            meritCount = flourishEffect and flourishEffect:getSubPower() or 0,
        })
    end

    -- TODO: it is unknown if ws attack mod and flourish bonus are additive or multiplicative
    local actorAttack = xi.combat.physical.rangedActorAttackFromParams({
        ratt               = actor:getStat(xi.mod.RATT),
        bonusRangedAttack  = bonusRangedAttack,
        distancePenalty    = distancePenalty,
        wsAttackMod        = wsAttackMod,
        flourishBonus      = flourishBonus,
    })

    -- Target Defense Modifiers.
    targetDefense = xi.combat.physical.effectiveDefenseFromParams({
        defense        = targetDefense,
        ignoresDefense = tpIgnoresDefense,
        ignoreFraction = tpFactor,
    })

    -- Mod-based bypass for ranged level correction
    if actor:isPC() and actor:getMod(xi.mod.RA_IGNORE_LVL_DIFF) > 0 then
        applyLevelCorrection = false
    end

    local inject =
    {
        actorAttack          = actorAttack,
        targetDefense        = targetDefense,
        isCritical           = isCritical,
        applyLevelCorrection = applyLevelCorrection,
        actorIsPC            = actor:isPC(),
        actorLevel           = actor:getMainLvl(),
        targetLevel          = target:getMainLvl(),
        weaponCap            = xi.combat.physical.weaponCap(weaponType),
        damageLimit          = actor:getMod(xi.mod.DAMAGE_LIMIT),
        damageLimitP         = actor:getMod(xi.mod.DAMAGE_LIMITP),
        critDmgIncrease      = actor:getMod(xi.mod.CRIT_DMG_INCREASE)
            + actor:getMod(xi.mod.RANGED_CRIT_DMG_INCREASE),
        critDefBonus         = target:getMod(xi.mod.CRIT_DEF_BONUS),
    }

    local lower, upper = xi.combat.physical.rangedPDIFBoundsFromParams(inject)
    inject.ratioRollValid = true
    inject.ratioRoll = math.random(lower * 1000, upper * 1000)
    return xi.combat.physical.rangedPDIFFromParams(inject)
end

-----------------------------------
-- Critical hit rate operations
-----------------------------------
-- dStat: Critical hit rate bonus from DEX vs AGI difference.
-----------------------------------
-- Pure critical-rate helpers (OmegaXI slice 6686)
-- Dual-wired to internal/critrate (6079 / 6212).
-----------------------------------

xi.combat.physical.baseCriticalRate = 0.05
xi.combat.physical.criticalRateFloor = 0.05
xi.combat.physical.criticalRateCap   = 1.0
xi.combat.physical.inninBehindAngle  = 23

-- Pure dDEX critical bonus once actorDEX - targetAGI is known.
xi.combat.physical.criticalRateFromStatDiffValue = function(dDex)
    dDex = dDex or 0

    if dDex > 50 then
        return 0.15
    elseif dDex >= 40 then
        return (dDex - 35) / 100
    elseif dDex >= 30 then
        return 0.04
    elseif dDex >= 20 then
        return 0.03
    elseif dDex >= 14 then
        return 0.02
    elseif dDex >= 7 then
        return 0.01
    end

    return 0
end

-- Pure dAGI critical bonus once max(0, actorAGI - targetAGI) is known.
xi.combat.physical.criticalRateFromAGIDiffValue = function(dAgi)
    dAgi = dAgi or 0

    if dAgi < 0 then
        dAgi = 0
    end

    return math.floor(dAgi / 10) / 100
end

-- Pure Innin bonus once status/position injects are known.
xi.combat.physical.criticalRateFromInninValue = function(hasInnin, isBehind, power)
    if not hasInnin or not isBehind then
        return 0
    end

    return power or 0
end

-- Pure Fencer gear eligibility (slice 6212 / 6686).
-- PC only; main equipped; not two-handed; not hand-to-hand;
-- sub empty OR skill NONE OR is shield.
xi.combat.physical.fencerEligible = function(isPC, hasMain, mainTwoHanded, mainHandToHand, hasSub, subSkillType, subIsShield)
    if not isPC or not hasMain then
        return false
    end

    if mainTwoHanded or mainHandToHand then
        return false
    end

    if not hasSub then
        return true
    end

    if (subSkillType or 0) == 0 then -- xi.skill.NONE
        return true
    end

    return subIsShield
end

-- Pure Fencer crit bonus once eligibility and FENCER_CRITHITRATE mod are known.
xi.combat.physical.criticalRateFromFencerValue = function(eligible, fencerMod)
    if not eligible then
        return 0
    end

    return (fencerMod or 0) / 100
end

-- Pure Building Flourish crit bonus once effect power/subPower are known.
xi.combat.physical.criticalRateFromFlourishValue = function(hasEffect, power, subPower)
    if not hasEffect or (power or 0) < 3 then
        return 0
    end

    return (10 + (subPower or 0)) / 100
end

-- Pure weapon-slot-only crit mod / 100.
xi.combat.physical.criticalRateFromWeaponSlotValue = function(weaponOnlyCritMod)
    return (weaponOnlyCritMod or 0) / 100
end

-- Pure swing/ranged critical rate clamp after component injects.
xi.combat.physical.clampCriticalRate = function(rate)
    return utils.clamp(rate, xi.combat.physical.criticalRateFloor, xi.combat.physical.criticalRateCap)
end

-----------------------------------
-- Pure: full swing/ranged critical-rate product (slice 6764 / 6079)
-- Parity: internal/critrate SwingRate / RangedRate
-----------------------------------
-- params: statBonus, inninBonus, fencerBonus, flourishBonus, weaponSlot,
--   modifierBonus, meritBonus, targetEvasion, targetMerit, tpFactor
xi.combat.physical.criticalRateFromParams = function(params)
    params = params or {}
    local rate = xi.combat.physical.baseCriticalRate
        + (params.statBonus or 0)
        + (params.inninBonus or 0)
        + (params.fencerBonus or 0)
        + (params.flourishBonus or 0)
        + (params.weaponSlot or 0)
        + (params.modifierBonus or 0)
        + (params.meritBonus or 0)
        - (params.targetEvasion or 0)
        - (params.targetMerit or 0)
        + (params.tpFactor or 0)

    return xi.combat.physical.clampCriticalRate(rate)
end

-----------------------------------
-- Entity hosts for critical-rate components
-----------------------------------

xi.combat.physical.criticalRateFromStatDiff = function(actor, target)
    local dDex = actor:getStat(xi.mod.DEX) - target:getStat(xi.mod.AGI)

    return xi.combat.physical.criticalRateFromStatDiffValue(dDex)
end

-- dStat: Ranged critical hit rate bonus from AGI vs AGI difference.
xi.combat.physical.criticalRateFromAGIDiff = function(actor, target)
    local dAgi = math.max(0, actor:getStat(xi.mod.AGI) - target:getStat(xi.mod.AGI))

    return xi.combat.physical.criticalRateFromAGIDiffValue(dAgi)
end

-- Innin: Critical hit rate bonus when actor is behind target.
xi.combat.physical.criticalRateFromInnin = function(actor, target)
    local hasInnin = actor:hasStatusEffect(xi.effect.INNIN)
    local power = 0

    if hasInnin then
        power = actor:getStatusEffect(xi.effect.INNIN):getPower()
    end

    return xi.combat.physical.criticalRateFromInninValue(
        hasInnin,
        actor:isBehind(target, xi.combat.physical.inninBehindAngle),
        power
    )
end

-- Fencer: Critical hit rate bonus when actor is only wielding with main hand.
xi.combat.physical.criticalRateFromFencer = function(actor)
    -- TODO: do any Trusts or mobs ever get Fencer bonuses?
    local isPC = actor:getObjType() == xi.objType.PC
    local mainEquip = isPC and actor:getStorageItem(0, 0, xi.slot.MAIN) or nil
    local subEquip  = isPC and actor:getStorageItem(0, 0, xi.slot.SUB) or nil
    local hasMain = mainEquip ~= nil
    local hasSub = subEquip ~= nil
    local main2H = hasMain and mainEquip:isTwoHanded()
    local mainH2H = hasMain and mainEquip:isHandToHand()
    local subSkill = hasSub and subEquip:getSkillType() or 0
    local subShield = hasSub and subEquip:isShield()

    local eligible = xi.combat.physical.fencerEligible(
        isPC, hasMain, main2H, mainH2H, hasSub, subSkill, subShield
    )

    return xi.combat.physical.criticalRateFromFencerValue(
        eligible,
        actor:getMod(xi.mod.FENCER_CRITHITRATE)
    )
end

-- Critical rate from Building Flourish.
-- TODO: Study case where if we can attach modifiers to the effect itself, both this and the effect may need refactoring.
xi.combat.physical.criticalRateFromFlourish = function(actor)
    local hasEffect = actor:hasStatusEffect(xi.effect.BUILDING_FLOURISH)
    local power = 0
    local subPower = 0

    if hasEffect then
        local effect = actor:getStatusEffect(xi.effect.BUILDING_FLOURISH)
        power = effect:getPower()
        subPower = effect:getSubPower()
    end

    return xi.combat.physical.criticalRateFromFlourishValue(hasEffect, power, subPower)
end

---@param actor CBaseEntity
---@param slot xi.slot
---@return number
xi.combat.physical.criticalRateFromWeaponSlot = function(actor, slot)
    if actor:isPC() then
        return xi.combat.physical.criticalRateFromWeaponSlotValue(
            actor:getGearModFromSlot(slot, xi.mod.CRITHITRATE_ONLY_WEP)
        )
    end

    return 0
end

-- Critical rate master function.
---@param actor CBaseEntity
---@param target CBaseEntity
---@param actorTP number
---@param slot xi.slot
---@param optCritModTable table?
---@return integer
-- Host residual: entity dDEX/Innin/Fencer/Flourish/slot/mod/merit/TP injects.
-- Pure product: criticalRateFromParams (slice 6764). StatBonus uses dDEX ladder.
xi.combat.physical.calculateSwingCriticalRate = function(actor, target, actorTP, slot, optCritModTable)
    -- See reference at https://www.bg-wiki.com/ffxi/Critical_Hit_Rate
    local tpFactor = 0
    if optCritModTable then
        tpFactor = xi.combat.physical.calculateTPfactor(actorTP, optCritModTable)
    end

    return xi.combat.physical.criticalRateFromParams({
        statBonus     = xi.combat.physical.criticalRateFromStatDiff(actor, target),
        inninBonus    = xi.combat.physical.criticalRateFromInnin(actor, target),
        fencerBonus   = xi.combat.physical.criticalRateFromFencer(actor),
        flourishBonus = xi.combat.physical.criticalRateFromFlourish(actor),
        weaponSlot    = xi.combat.physical.criticalRateFromWeaponSlot(actor, slot),
        modifierBonus = actor:getMod(xi.mod.CRITHITRATE) / 100,
        meritBonus    = actor:getMerit(xi.merit.CRIT_HIT_RATE) / 100,
        targetEvasion = target:getMod(xi.mod.CRITICAL_HIT_EVASION) / 100,
        targetMerit   = target:getMerit(xi.merit.ENEMY_CRIT_RATE) / 100,
        tpFactor      = tpFactor,
    }) -- TODO: Need confirmation of no upper cap.
end

---@param actor CBaseEntity
---@param target CBaseEntity
---@param actorTP number
---@param slot xi.slot
---@param optCritModTable table?
---@return integer
-- Host residual: entity dAGI/Innin/Fencer/Flourish/slot/mod/merit/TP injects.
-- Pure product: criticalRateFromParams (slice 6764). StatBonus uses dAGI ladder.
xi.combat.physical.calculateRangedCriticalRate = function(actor, target, actorTP, slot, optCritModTable)
    -- See reference at https://www.bg-wiki.com/ffxi/Critical_Hit_Rate
    local tpFactor = 0
    if optCritModTable then
        tpFactor = xi.combat.physical.calculateTPfactor(actorTP, optCritModTable)
    end

    return xi.combat.physical.criticalRateFromParams({
        statBonus     = xi.combat.physical.criticalRateFromAGIDiff(actor, target),
        inninBonus    = xi.combat.physical.criticalRateFromInnin(actor, target),
        fencerBonus   = xi.combat.physical.criticalRateFromFencer(actor),
        flourishBonus = xi.combat.physical.criticalRateFromFlourish(actor),
        weaponSlot    = xi.combat.physical.criticalRateFromWeaponSlot(actor, slot),
        modifierBonus = actor:getMod(xi.mod.CRITHITRATE) / 100,
        meritBonus    = actor:getMerit(xi.merit.CRIT_HIT_RATE) / 100,
        targetEvasion = target:getMod(xi.mod.CRITICAL_HIT_EVASION) / 100,
        targetMerit   = target:getMerit(xi.merit.ENEMY_CRIT_RATE) / 100,
        tpFactor      = tpFactor,
    }) -- TODO: Need confirmation of no upper cap.
end

xi.combat.physical.calculateNumberOfHits = function(actor, additionalParamsHere)
end

-- Main Hit (First hit) Functions.
xi.combat.physical.calculateMainHitAccuracy = function(actor, additionalParamsHere)
end

xi.combat.physical.calculateMainHitCritical = function(actor, additionalParamsHere)
end

xi.combat.physical.calculateMainHitDamage = function(actor, additionalParamsHere)
end

-- Secondary Hits (All other) Functions.
xi.combat.physical.calculateSecondaryHitAccuracy = function(actor, additionalParamsHere)
end

xi.combat.physical.calculateSecondaryHitCritical = function(actor, additionalParamsHere)
end

xi.combat.physical.calculateSecondaryHitDamage = function(actor, additionalParamsHere)
end

-----------------------------------
-- Pure parry / guard helpers (OmegaXI slice 6689)
-- Dual-wired to internal/parryrate and internal/guardrate.
-- Spreadsheet: https://docs.google.com/spreadsheets/d/1wtS0d4nNqosMwFuHWb7fkCcj4naAvwhnPocf3qSGJjk
-----------------------------------

xi.combat.physical.parryRateMin = 5
xi.combat.physical.parryRateMax = 25
xi.combat.physical.guardRateMin = 5
xi.combat.physical.guardRateMax = 25
xi.combat.physical.parrySkillDeltaPivot = 5
xi.combat.physical.parrySkillDeltaOffset = 6
xi.combat.physical.guardSkillDeltaPivot = 6
xi.combat.physical.defenseRateBase = 10
xi.combat.physical.defenseLowBranchDivisor = 36 / 9  -- 4
xi.combat.physical.defenseHighBranchDivisor = 60 / 9 -- 6.666...

-- Pure canParry after facing/engaged/prevent/equip/mobmod injects.
xi.combat.physical.canParryFromParams = function(params)
    if not params.facing or not params.engaged or params.preventAction then
        return false
    end

    if params.isPC then
        if (params.parrySkillRank or 0) == 0 then
            return false
        end

        if not params.hasMainWeapon then
            return false
        end

        return params.mainSkillType ~= xi.skill.HAND_TO_HAND
    end

    return (params.canParryMobMod or 0) > 0
end

-- Pure canGuard after facing/engaged/prevent/job/equip injects.
xi.combat.physical.canGuardFromParams = function(params)
    if not params.facing or not params.engaged or params.preventAction then
        return false
    end

    if params.isPC then
        if (params.guardSkillRank or 0) <= 0 then
            return false
        end

        -- (not mainWeapon) or skill == H2H
        return (not params.hasMainWeapon) or params.mainSkillType == xi.skill.HAND_TO_HAND
    end

    if not params.isMobPetOrTrust then
        return false
    end

    if params.mainJob ~= xi.job.MNK and params.mainJob ~= xi.job.PUP then
        return false
    end

    return (params.cannotGuardMod or 0) == 0
end

-- Pure PC defender parry skill assembly.
xi.combat.physical.defenderParrySkillPC = function(parrySkill, parryMod, iLvlParry)
    return (parrySkill or 0) + (parryMod or 0) + (iLvlParry or 0)
end

-- Pure PC defender guard skill assembly (getILvlParry also covers H2H guard).
xi.combat.physical.defenderGuardSkillPC = function(guardSkill, guardMod, iLvlParry)
    return (guardSkill or 0) + (guardMod or 0) + (iLvlParry or 0)
end

-- Pure PC attacker main-weapon skill assembly.
xi.combat.physical.attackerWeaponSkillPC = function(weaponSkill, iLvlSkill)
    return (weaponSkill or 0) + (iLvlSkill or 0)
end

-- Pure calculateParryRate once skills and flat bonuses are injected.
-- params: defenderSkill, attackerSkill, issekiganPower, inquartataMod
xi.combat.physical.parryRateFromParams = function(params)
    local defenderSkill = params.defenderSkill or 0
    local attackerSkill = params.attackerSkill or 0
    local skillDelta = defenderSkill - attackerSkill
    local parryRate

    if skillDelta <= xi.combat.physical.parrySkillDeltaPivot then
        parryRate = math.floor(
            xi.combat.physical.defenseRateBase +
            (skillDelta - xi.combat.physical.parrySkillDeltaOffset) / xi.combat.physical.defenseLowBranchDivisor
        )
    else
        parryRate = math.floor(
            xi.combat.physical.defenseRateBase +
            (skillDelta - xi.combat.physical.parrySkillDeltaOffset) / xi.combat.physical.defenseHighBranchDivisor
        )
    end

    parryRate = utils.clamp(parryRate, xi.combat.physical.parryRateMin, xi.combat.physical.parryRateMax)
    parryRate = parryRate + (params.issekiganPower or 0) + (params.inquartataMod or 0)

    return parryRate
end

-- Pure calculateGuardRate once skills and additive guard are injected.
-- params: defenderSkill, attackerSkill, additiveGuard
xi.combat.physical.guardRateFromParams = function(params)
    local defenderSkill = params.defenderSkill or 0
    local attackerSkill = params.attackerSkill or 0
    local skillDelta = defenderSkill - attackerSkill
    local guardRate

    if skillDelta <= xi.combat.physical.guardSkillDeltaPivot then
        guardRate = math.floor(
            xi.combat.physical.defenseRateBase + skillDelta / xi.combat.physical.defenseLowBranchDivisor
        )
    else
        guardRate = math.floor(
            xi.combat.physical.defenseRateBase + skillDelta / xi.combat.physical.defenseHighBranchDivisor
        )
    end

    -- Dodge's guard bonus goes over the cap
    guardRate = utils.clamp(guardRate, xi.combat.physical.guardRateMin, xi.combat.physical.guardRateMax) +
        (params.additiveGuard or 0)

    return guardRate
end

xi.combat.physical.parrySucceeds = function(rate, roll)
    return (rate or 0) * 100 >= (roll or 0)
end

xi.combat.physical.guardSucceeds = function(rate, roll)
    return (rate or 0) * 100 >= (roll or 0)
end

-----------------------------------
-- Entity hosts for parry / guard
-----------------------------------

xi.combat.physical.canParry = function(defender, attacker)
    local isPC = defender:isPC()
    local hasMain = false
    local mainSkillType = 0
    local parryRank = 0
    local canParryMod = 0

    if isPC then
        parryRank = defender:getSkillRank(xi.skill.PARRY)
        local mainWeapon = defender:getEquippedItem(xi.slot.MAIN)
        if mainWeapon then
            hasMain = true
            mainSkillType = mainWeapon:getSkillType()
        end
    else
        canParryMod = defender:getMobMod(xi.mobMod.CAN_PARRY)
    end

    return xi.combat.physical.canParryFromParams({
        facing         = defender:isFacing(attacker),
        engaged        = defender:isEngaged(),
        preventAction  = defender:hasPreventActionEffect(true),
        isPC           = isPC,
        parrySkillRank = parryRank,
        hasMainWeapon  = hasMain,
        mainSkillType  = mainSkillType,
        canParryMobMod = canParryMod,
    })
end

-- Rough implementation of the sheet (still not 100% known).
xi.combat.physical.calculateParryRate = function(defender, attacker)
    local defenderSkill
    local attackerSkill

    if defender:isPC() then
        defenderSkill = xi.combat.physical.defenderParrySkillPC(
            defender:getSkillLevel(xi.skill.PARRY),
            defender:getMod(xi.mod.PARRY),
            defender:getILvlParry()
        )
    else
        defenderSkill = xi.data.skillLevel.getSkillCap(defender:getMainLvl(), xi.skillRank.A_PLUS)
    end

    if attacker:isPC() then
        attackerSkill = xi.combat.physical.attackerWeaponSkillPC(
            attacker:getSkillLevel(attacker:getWeaponSkillType(xi.slot.MAIN)),
            attacker:getILvlSkill()
        )
    else
        attackerSkill = xi.data.skillLevel.getSkillCap(attacker:getMainLvl(), xi.skillRank.A_PLUS)
    end

    local issekiganPower = 0
    if defender:hasStatusEffect(xi.effect.ISSEKIGAN) then
        issekiganPower = defender:getStatusEffect(xi.effect.ISSEKIGAN):getPower()
    end

    return xi.combat.physical.parryRateFromParams({
        defenderSkill  = defenderSkill,
        attackerSkill  = attackerSkill,
        issekiganPower = issekiganPower,
        inquartataMod  = defender:getMod(xi.mod.INQUARTATA),
    })
end

xi.combat.physical.canGuard = function(defender, attacker)
    local isPC = defender:isPC()
    local isMobPetOrTrust = defender:isMob() or defender:isPet() or defender:isTrust()
    local hasMain = false
    local mainSkillType = 0
    local guardRank = 0
    local mainJob = 0
    local cannotGuard = 0

    if isPC then
        guardRank = defender:getSkillRank(xi.skill.GUARD)
        local mainWeapon = defender:getEquippedItem(xi.slot.MAIN)
        if mainWeapon then
            hasMain = true
            mainSkillType = mainWeapon:getSkillType()
        end
    elseif isMobPetOrTrust then
        mainJob = defender:getMainJob()
        cannotGuard = defender:getMobMod(xi.mobMod.CANNOT_GUARD)
    end

    return xi.combat.physical.canGuardFromParams({
        facing           = defender:isFacing(attacker),
        engaged          = defender:isEngaged(),
        preventAction    = defender:hasPreventActionEffect(true),
        isPC             = isPC,
        isMobPetOrTrust  = isMobPetOrTrust,
        guardSkillRank   = guardRank,
        hasMainWeapon    = hasMain,
        mainSkillType    = mainSkillType,
        mainJob          = mainJob,
        cannotGuardMod   = cannotGuard,
    })
end

xi.combat.physical.calculateGuardRate = function(defender, attacker)
    local defenderSkill
    local attackerSkill

    if defender:isPC() then
        defenderSkill = xi.combat.physical.defenderGuardSkillPC(
            defender:getSkillLevel(xi.skill.GUARD),
            defender:getMod(xi.mod.GUARD),
            defender:getILvlParry() -- also gets guard (h2h cannot have parry on the weapon)
        )
    else
        defenderSkill = xi.data.skillLevel.getSkillCap(defender:getMainLvl(), xi.skillRank.A_PLUS)
    end

    if attacker:isPC() then
        attackerSkill = xi.combat.physical.attackerWeaponSkillPC(
            attacker:getSkillLevel(attacker:getWeaponSkillType(xi.slot.MAIN)),
            attacker:getILvlSkill()
        )
    else
        attackerSkill = xi.data.skillLevel.getSkillCap(attacker:getMainLvl(), xi.skillRank.A_PLUS)
    end

    return xi.combat.physical.guardRateFromParams({
        defenderSkill = defenderSkill,
        attackerSkill = attackerSkill,
        additiveGuard = defender:getMod(xi.mod.ADDITIVE_GUARD),
    })
end

-----------------------------------
-- Pure shield block helpers (OmegaXI slice 6688)
-- Dual-wired to internal/blockrate.
-----------------------------------

xi.combat.physical.blockRateFromShieldSize = function(shieldSize)
    return xi.combat.physical.shieldSizeToBlockRateTable[shieldSize] or 0
end

-- Pure canBlock after facing/prevent/equip/mobmod injects.
-- params: facing, preventAction, isPC, isMobPetOrTrust,
--   shieldSkillRank, hasSubItem, subIsShield, canShieldBlockMod
xi.combat.physical.canBlockFromParams = function(params)
    if not params.facing or params.preventAction then
        return false
    end

    if params.isPC then
        if (params.shieldSkillRank or 0) <= 0 then
            return false
        end

        return params.hasSubItem and params.subIsShield
    end

    if not params.isMobPetOrTrust then
        return false
    end

    return (params.canShieldBlockMod or 0) > 0
end

-- Pure calculateBlockRate once kind and skills are injected.
-- kind: 'pc' | 'mob' | 'automaton'
-- params: kind, hasShield, shieldSize, canShieldBlock, baseBlockRate,
--   blockSkill, attackSkill, automatonMeleeSkill, palisadeMod,
--   hasReprisal, reprisalBlockBonus
xi.combat.physical.blockRateFromParams = function(params)
    local kind = params.kind or 'mob'
    local attackSkill = params.attackSkill or 0
    local palisade = params.palisadeMod or 0

    if kind == 'pc' then
        if not params.hasShield then
            return 0
        end

        local blockRate = xi.combat.physical.blockRateFromShieldSize(params.shieldSize or 0)
        local blockSkill = params.blockSkill or 0
        local reprisalMult = 1.0

        if params.hasReprisal then
            blockSkill = blockSkill * xi.combat.physical.reprisalSkillScale
            reprisalMult = xi.combat.physical.reprisalMultDefault

            if params.reprisalBlockBonus then
                reprisalMult = xi.combat.physical.reprisalMultBonus
            end
        end

        local skillModifier = (blockSkill - attackSkill) * xi.combat.physical.blockSkillDeltaCoeff

        return utils.clamp((blockRate + skillModifier + palisade) * reprisalMult,
            xi.combat.physical.blockRateMin, xi.combat.physical.blockRateMax)
    end

    if not params.canShieldBlock then
        return 0
    end

    if kind == 'automaton' then
        local skillModifier = ((params.automatonMeleeSkill or 0) - attackSkill) *
            xi.combat.physical.automatonBlockSkillDeltaCoeff

        return math.max(0, (params.baseBlockRate or 0) + skillModifier)
    end

    -- mob / pet / trust
    local blockRate = params.baseBlockRate or 0
    local blockSkill = params.blockSkill or 0
    local reprisalMult = 1.0

    if params.hasReprisal then
        blockSkill = blockSkill * xi.combat.physical.reprisalSkillScale
        reprisalMult = xi.combat.physical.reprisalMultDefault

        if params.reprisalBlockBonus then
            reprisalMult = xi.combat.physical.reprisalMultBonus
        end
    end

    local skillModifier = (blockSkill - attackSkill) * xi.combat.physical.blockSkillDeltaCoeff

    return utils.clamp((blockRate + skillModifier + palisade) * reprisalMult,
        xi.combat.physical.blockRateMin, xi.combat.physical.blockRateMax)
end

-- Pure getDamageReductionForBlock after shield def / absorb injects.
-- Returns flat damage reduction (original - remaining).
xi.combat.physical.damageReductionForBlockFromParams = function(params)
    local originalDamage = params.damage or 0

    if originalDamage <= 0 then
        return 0
    end

    local damage = math.max(0, originalDamage - (params.shieldDefBonus or 0))

    if params.isPC then
        local absorb = utils.clamp(100 - (params.shieldAbsorbRate or 0), 0, 100)
        damage = math.floor(damage * (absorb / 100))
    else
        damage = math.floor(damage * xi.combat.physical.nonPCBlockAbsorbFraction)
    end

    return originalDamage - damage
end

-- Pure isBlocked roll: rate * 100 >= roll (roll in 1..10000).
xi.combat.physical.blockSucceeds = function(rate, roll)
    return (rate or 0) * 100 >= (roll or 0)
end

-- Pure attacker skill type for block rate (H2H default vs main weapon).
xi.combat.physical.attackerSkillTypeForBlock = function(usingH2H, mainWeaponSkillType)
    if usingH2H then
        return xi.skill.HAND_TO_HAND
    end

    return mainWeaponSkillType
end

-----------------------------------
-- Entity hosts for shield block
-----------------------------------

xi.combat.physical.canBlock = function(defender, attacker)
    local isPC = defender:isPC()
    local isMobPetOrTrust = defender:isMob() or defender:isPet() or defender:isTrust()
    local hasSub = false
    local subIsShield = false
    local shieldRank = 0

    if isPC then
        shieldRank = defender:getSkillRank(xi.skill.SHIELD)
        local shield = defender:getEquippedItem(xi.slot.SUB)
        if shield then
            hasSub = true
            subIsShield = shield:isShield()
        end
    end

    return xi.combat.physical.canBlockFromParams({
        facing             = defender:isFacing(attacker),
        preventAction      = defender:hasPreventActionEffect(true),
        isPC               = isPC,
        isMobPetOrTrust    = isMobPetOrTrust,
        shieldSkillRank    = shieldRank,
        hasSubItem         = hasSub,
        subIsShield        = subIsShield,
        canShieldBlockMod  = isMobPetOrTrust and defender:getMobMod(xi.mobMod.CAN_SHIELD_BLOCK) or 0,
    })
end

xi.combat.physical.calculateBlockRate = function(defender, attacker)
    local attackerSkillType = xi.combat.physical.attackerSkillTypeForBlock(
        attacker:isUsingH2H(),
        attacker:getWeaponSkillType(xi.slot.MAIN)
    )
    local attackSkill = attacker:getSkillLevel(attackerSkillType)
    local palisadeMod = defender:getMod(xi.mod.PALISADE_BLOCK_BONUS)
    local hasReprisal = defender:hasStatusEffect(xi.effect.REPRISAL)
    local reprisalBonus = defender:getMod(xi.mod.REPRISAL_BLOCK_BONUS) > 0

    if defender:isPC() then
        local shield = defender:getEquippedItem(xi.slot.SUB)
        if not (shield and shield:isShield()) then
            return 0
        end

        return xi.combat.physical.blockRateFromParams({
            kind               = 'pc',
            hasShield          = true,
            shieldSize         = shield:getShieldSize(),
            blockSkill         = defender:getSkillLevel(xi.skill.SHIELD),
            attackSkill        = attackSkill,
            palisadeMod        = palisadeMod,
            hasReprisal        = hasReprisal,
            reprisalBlockBonus = reprisalBonus,
        })
    end

    if not (defender:isMob() or defender:isPet() or defender:isTrust()) then
        return 0
    end

    if defender:getMobMod(xi.mobMod.CAN_SHIELD_BLOCK) <= 0 then
        return 0
    end

    if defender:isAutomaton() then
        return xi.combat.physical.blockRateFromParams({
            kind                = 'automaton',
            canShieldBlock      = true,
            baseBlockRate       = defender:getMod(xi.mod.SHIELDBLOCKRATE),
            automatonMeleeSkill = defender:getSkillLevel(xi.skill.AUTOMATON_MELEE),
            attackSkill         = attackSkill,
        })
    end

    local blockSkill
    if defender:isTrust() then
        -- TODO: check trust type for ilvl > 99 when implemented
        blockSkill = defender:getMaxSkillLevel(
            math.min(defender:getMainLvl(), xi.combat.physical.softMaxTrustLevel),
            defender:getMainJob(),
            xi.skill.SHIELD
        )
    else
        blockSkill = defender:getMaxSkillLevel(defender:getMainLvl(), defender:getMainJob(), xi.skill.SHIELD)
    end

    return xi.combat.physical.blockRateFromParams({
        kind               = 'mob',
        canShieldBlock     = true,
        baseBlockRate      = defender:getMod(xi.mod.SHIELDBLOCKRATE),
        blockSkill         = blockSkill,
        attackSkill        = attackSkill,
        palisadeMod        = palisadeMod,
        hasReprisal        = hasReprisal,
        reprisalBlockBonus = reprisalBonus,
    })
end

xi.combat.physical.getDamageReductionForBlock = function(defender, attacker, damage)
    local isPC = defender:isPC()
    local absorbRate = 0

    if isPC then
        local shield = defender:getEquippedItem(xi.slot.SUB)
        absorbRate = shield:getShieldAbsorptionRate()
    end

    return xi.combat.physical.damageReductionForBlockFromParams({
        damage           = damage,
        shieldDefBonus   = defender:getMod(xi.mod.SHIELD_DEF_BONUS),
        isPC             = isPC,
        shieldAbsorbRate = absorbRate,
    })
end

xi.combat.physical.isBlocked = function(defender, attacker)
    local blocked = false

    if xi.combat.physical.canBlock(defender, attacker) then
        if xi.combat.physical.blockSucceeds(
            xi.combat.physical.calculateBlockRate(defender, attacker),
            math.random(1, 10000)
        ) then
            blocked = true
        end

        -- Handle skill ups.
        if
            defender:isPC() and
            (blocked or                                  -- We blocked
            not xi.settings.map.DEFENSIVE_OLD_SKILLUP_STYLE) -- Old style skillup is not enabled
        then
            defender:trySkillUp(xi.skill.SHIELD, attacker:getMainLvl())
        end
    end

    return blocked
end

xi.combat.physical.isParried = function(defender, attacker)
    local parried = false

    if xi.combat.physical.canParry(defender, attacker) then
        local isPC = defender:isPC()

        if xi.combat.physical.parrySucceeds(xi.combat.physical.calculateParryRate(defender, attacker), math.random(1, 10000)) then
            parried = true

            -- https://www.bg-wiki.com/ffxi/Turms_Mittens
            if
                defender:getMod(xi.mod.PARRY_HP_RECOVERY) > 0 and
                not defender:hasStatusEffect(xi.effect.CURSE_II)
            then
                local recoveryValue = defender:getMod(xi.mod.PARRY_HP_RECOVERY)
                defender:addHP(recoveryValue)
            end

            if isPC then
                -- handle tactical parry
                if defender:hasTrait(xi.trait.TACTICAL_PARRY) then
                    defender:addTP(defender:getMod(xi.mod.TACTICAL_PARRY))
                end
            end
        end

        -- Handle skill ups.
        if
            isPC and
            (parried or                                  -- We parried
            not xi.settings.map.DEFENSIVE_OLD_SKILLUP_STYLE) -- Old style skillup is not enabled
        then
            defender:trySkillUp(xi.skill.PARRY, attacker:getMainLvl())
        end
    end

    return parried
end

xi.combat.physical.isGuarded = function(defender, attacker)
    local guarded = false

    if xi.combat.physical.canGuard(defender, attacker) then
        local isPC = defender:isPC()
        if xi.combat.physical.guardSucceeds(xi.combat.physical.calculateGuardRate(defender, attacker), math.random(1, 10000)) then
            guarded = true
            if isPC then
                -- handle tactical guard
                if defender:hasTrait(xi.trait.TACTICAL_GUARD) then
                    defender:addTP(defender:getMod(xi.mod.TACTICAL_GUARD))
                end
            end
        end

        -- Handle skill ups.
        if
            isPC and
            (guarded or                                  -- We guarded
            not xi.settings.map.DEFENSIVE_OLD_SKILLUP_STYLE) -- Old style skillup is not enabled
        then
            defender:trySkillUp(xi.skill.GUARD, attacker:getMainLvl())
        end
    end

    return guarded
end
