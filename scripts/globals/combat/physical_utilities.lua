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
local wsElementalProperties =
{
    -- [Skillchain type             ] = { Fire, Ice, Wind, Earth, Thunder, Water, Light, Dark },
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
    local bonusBasePhysicalDamage = 0
    local damage                  = 0

    -- Sneak Attack
    if isSneakAttack then
        bonusBasePhysicalDamage = math.floor(bonusBasePhysicalDamage + actor:getStat(xi.mod.DEX) * (1 + actor:getMod(xi.mod.SNEAK_ATK_DEX) / 100))
    end

    -- Trick Attack
    if isTrickAttack then
        bonusBasePhysicalDamage = math.floor(bonusBasePhysicalDamage + actor:getStat(xi.mod.AGI) * (1 + actor:getMod(xi.mod.TRICK_ATK_AGI) / 100))
    end

    -- Consume Mana
    bonusBasePhysicalDamage = bonusBasePhysicalDamage + xi.combat.damage.consumeManaAddition(actor)

    -- Apply damage ratio multiplier.
    local baseDamage = 0

    if isH2H then
        local naturalH2hDamage = math.floor(actor:getSkillLevel(xi.skill.HAND_TO_HAND) * 0.11) + 3

        if actor:isMob() then
            local mobH2HPenalty = 1
            local regionID      = actor:getCurrentRegion()
            local fSTR          = xi.combat.physical.calculateMeleeStatFactor(actor, target)

            if actor:getMobMod(xi.mobMod.NO_H2H_PENALTY) == 0 then
                if regionID <= xi.region.LIMBUS then
                    mobH2HPenalty = 0.425 -- Vanilla - COP
                else
                    mobH2HPenalty = 0.65
                end
            end

            baseDamage = actor:getWeaponDmg() + bonusBasePhysicalDamage

            if physicalAttackType == xi.physicalAttackType.KICK then
                local kickPenalty = 2 / 3 -- Per Jimmy, kicks get a second penalty, then fSTR is added
                local kickDamage  = actor:getMod(xi.mod.KICK_DMG)

                -- Per Jimmy, kick damage penalty for mobs can only be damage * h2h penalty * kickpenalty + fstr
                -- The math doesn't work in any other way, which is strange given fSTR is before the penalty on non-kicks
                baseDamage = (baseDamage + kickDamage) * mobH2HPenalty * kickPenalty + fSTR
            else
                baseDamage = (baseDamage + fSTR) * mobH2HPenalty
            end
        elseif physicalAttackType == xi.physicalAttackType.KICK then
            baseDamage = naturalH2hDamage + actor:getMod(xi.mod.KICK_DMG) + bonusBasePhysicalDamage + xi.combat.physical.calculateMeleeStatFactor(actor, target)
        else
            baseDamage = naturalH2hDamage + actor:getWeaponDmg() + bonusBasePhysicalDamage + xi.combat.physical.calculateMeleeStatFactor(actor, target)
        end
    elseif slot == xi.slot.MAIN then
        baseDamage = actor:getWeaponDmg() + bonusBasePhysicalDamage + xi.combat.physical.calculateMeleeStatFactor(actor, target)
    elseif slot == xi.slot.SUB then
        baseDamage = actor:getOffhandDmg() + bonusBasePhysicalDamage + xi.combat.physical.calculateMeleeStatFactor(actor, target)
    elseif slot == xi.slot.AMMO then
        baseDamage = actor:getRangedDmg() + xi.combat.physical.calculateRangedStatFactor(actor, target)
    end

    damage = math.floor(baseDamage * damageRatio)

    -- Scarlet Delirium multiplier.
    damage = math.floor(damage * xi.combat.damage.scarletDeliriumMultiplier(actor))

    -- Double/Triple Attack multipliers.
    local multiAttackMultiplier = 1
    if physicalAttackType == xi.physicalAttackType.DOUBLE then
        multiAttackMultiplier = 1 + actor:getMod(xi.mod.DOUBLE_ATTACK_DMG) / 100
    elseif physicalAttackType == xi.physicalAttackType.TRIPLE then
        multiAttackMultiplier = 1 + actor:getMod(xi.mod.TRIPLE_ATTACK_DMG) / 100
    end

    damage = math.floor(damage * multiAttackMultiplier)

    -- Soul Eater additive damage.
    damage = damage + xi.combat.damage.souleaterAddition(actor)

    -- Damage multipliers
    damage = actor:addDamageFromMultipliers(damage, physicalAttackType, slot, isFirstSwing)

    -- Sneak Attack Augment
    if
        actor:getMod(xi.mod.AUGMENTS_SA) > 0 and
        isSneakAttack and
        actor:hasStatusEffect(xi.effect.SNEAK_ATTACK)
    then
        damage = math.floor(damage * (1 + actor:getMod(xi.mod.AUGMENTS_SA) / 100))
    end

    -- Trick Attack Augment
    if
        actor:getMod(xi.mod.AUGMENTS_TA) > 0 and
        isTrickAttack and
        actor:hasStatusEffect(xi.effect.TRICK_ATTACK)
    then
        damage = math.floor(damage * (1 + actor:getMod(xi.mod.AUGMENTS_TA) / 100))
    end

    --- Low level mobs can get negative fSTR so low they crater their (base weapon damage + fstr) to below 0.
    --- Absorption isn't possible at this point in the calculation, so zero it.
    if damage < 0 then
        damage = 0
    end

    -- Apply Restraint Weaponskill Damage
    if
        isFirstSwing and
        actor:hasStatusEffect(xi.effect.RESTRAINT)
    then
        local effect = actor:getStatusEffect(xi.effect.RESTRAINT)
        local power  = effect and effect:getPower() or 30

        if
            effect and
            power < 30
        then
            local jpBonus = actor:getJobPointLevel(xi.jp.RESTRAINT_EFFECT) * 2

            -- Convert weapon delay and divide
            -- Pull remainder of previous hit's value from Effect Sub Power
            local boostPerRound = (3 * actor:getBaseDelay() / 50) / 385
            local remainder     = effect:getSubPower() / 100

            -- Calculate bonuses from Enhances Restraint, Job Point upgrades, and remainder from previous hit
            boostPerRound = remainder + boostPerRound * (1 + actor:getMod(xi.mod.ENHANCES_RESTRAINT) / 100) * (1 + jpBonus / 100)

            -- Calculate new remainder and multiply by 100 so significant digits aren't lost
            remainder     = math.floor((1 - math.ceil(boostPerRound) - boostPerRound) * 100)
            boostPerRound = math.floor(boostPerRound)

            -- Cap total power to +30% WSD
            if power + boostPerRound > 30 then
                boostPerRound = 30 - power
            end

            effect:setPower(power + boostPerRound)
            effect:setSubPower(remainder)
            actor:setMod(xi.mod.ALL_WSDMG_FIRST_HIT, boostPerRound)
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

-- TP Multiplier calculations.
xi.combat.physical.calculateFTPBonus = function(actor)
    local fTPBonus = 0

    -- Early return: Gear bonuses only come from gear.
    if actor:getObjType() ~= xi.objType.PC then
        return fTPBonus
    end

    -- Early return: Gear bonuses only apply to weaponskills with elemental properties.
    local scProp1, scProp2, scProp3 = actor:getWSSkillchainProp()
    if
        scProp1 == xi.skillchainType.NONE and
        scProp2 == xi.skillchainType.NONE and
        scProp3 == xi.skillchainType.NONE
    then
        return fTPBonus
    end

    -- fTP bonuses from gear.
    local dayElement = VanadielDayElement()

    for elementChecked = xi.element.FIRE, xi.element.DARK do
        if
            wsElementalProperties[scProp1][elementChecked] == 1 or
            wsElementalProperties[scProp2][elementChecked] == 1 or
            wsElementalProperties[scProp3][elementChecked] == 1
        then
            fTPBonus = fTPBonus + actor:getMod(xi.data.element.getElementalFTPModifier(elementChecked)) / 256

            if dayElement == elementChecked then
                fTPBonus = fTPBonus + actor:getMod(xi.mod.DAY_FTP_BONUS) / 256
            end
        end
    end

    fTPBonus = fTPBonus + actor:getMod(xi.mod.ANY_FTP_BONUS) / 256

    return fTPBonus
end

---@param wRatio number
---@param pDifFinalCap number
-- Pure PC wRatio → pDIF lower/upper caps (OmegaXI slice 6684 dual-wire / 0840).
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

local function getSpikeRatio(isPC, wRatio)
    return xi.combat.physical.spikeRatio(isPC, wRatio)
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
xi.combat.physical.calculateMeleePDIF = function(actor, target, weaponType, wsAttackMod, isCritical, applyLevelCorrection, tpIgnoresDefense, tpFactor, isWeaponskill, weaponSlot, isCannonball)
    local pDif = 0

    ----------------------------------------
    -- Step 1: Attack / Defense Ratio
    ----------------------------------------
    local baseRatio     = 0
    local actorAttack   = 0
    local targetDefense = math.max(1, target:getStat(xi.mod.DEF))
    local flourishBonus = 1

    -- Actor Weaponskill Specific Attack modifiers.
    if isWeaponskill then
        local flourishEffect = actor:getStatusEffect(xi.effect.BUILDING_FLOURISH)

        if flourishEffect and flourishEffect:getPower() >= 2 then -- 2 or more Finishing Moves used.
            local meritCount = flourishEffect:getSubPower()

            flourishBonus = 1.25 + 0.01 * meritCount -- +1% attack bonus per merit -- TODO: do the merits apply even when FMs are < 2?
        end
    end

    -- TODO: it is unknown if ws attack mod and flourish bonus are additive or multiplicative
    -- TODO: do flourish and attack mods come before or after food?
    actorAttack = math.max(1, math.floor(actor:getStat(xi.mod.ATT, weaponSlot) * wsAttackMod * flourishBonus))

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
    if tpIgnoresDefense then
        local ignoreDefenseFactor = 1 - tpFactor

        targetDefense = math.max(1, math.floor(targetDefense * ignoreDefenseFactor))
    end

    if isCannonball then
        actorAttack = actor:getStat(xi.mod.DEF)
    end

    -- Actor Attack / Target Defense ratio
    if targetDefense ~= 0 then
        baseRatio = actorAttack / targetDefense
    end

    ----------------------------------------
    -- Step 2: cRatio (Level correction, corrected ratio) Zone based!
    ----------------------------------------
    local levelDifFactor = 0

    -- https://forum.square-enix.com/ffxi/threads/31310-March-27-2013-(JST)-Version-Update
    -- TODO: There is some weirdness with needing 2 levels to start level correction in retail
    -- It is not currently implemented.
    if applyLevelCorrection then
        levelDifFactor = (actor:getMainLvl() - target:getMainLvl()) * 3 / 64 -- 3/64 from JP model which fits better
    end

    -- Only players suffer from negative level difference.
    if
        not actor:isPC() and
        levelDifFactor < 0
    then
        levelDifFactor = 0
    end

    -- Players do not get positive level correction, only monsters
    if
        actor:isPC() and
        levelDifFactor > 0
    then
        levelDifFactor = 0
    end

    ----------------------------------------
    -- Step 3: wRatio and pDif Caps (Melee)
    ----------------------------------------
    local wRatio             = baseRatio + (isCritical and 1 or 0)
    local pDifUpperCap       = 0
    local pDifLowerCap       = 0
    local damageLimitPlus    = actor:getMod(xi.mod.DAMAGE_LIMIT) / 100
    local damageLimitPercent = 1 + actor:getMod(xi.mod.DAMAGE_LIMITP) / 100
    local pDifFinalCap       = 0

    if actor:isPC() then
        pDifFinalCap = (xi.combat.physical.weaponCap(weaponType) + damageLimitPlus) * damageLimitPercent + (isCritical and 1 or 0)

        local sRatio = getSpikeRatio(true, wRatio)

        if math.random(1, 10000) / 10000 <= sRatio then
            return 1.0
        end

        pDifLowerCap, pDifUpperCap = xi.combat.physical.wRatioCapPC(wRatio, pDifFinalCap)
    else
        -- Mobs and pets, unconfirmed if pets use this same formula
        -- corrected mobs have 2.0 pdif + 1.0 for crits, with level correction added after the fact
        -- non-corrected mobs have 4.0 pdif cap, but there is some indication that ilvl may go up to 8.0
        local basePDIF  = applyLevelCorrection and 2 or 4
        local critBonus = (applyLevelCorrection and isCritical) and 1 or 0
        pDifFinalCap    = (basePDIF + damageLimitPlus) * damageLimitPercent + critBonus

        local sRatio = getSpikeRatio(false, wRatio)

        if math.random(1, 10000) / 10000 <= sRatio then
            return 1.0
        end

        pDifLowerCap, pDifUpperCap = xi.combat.physical.wRatioCapOthers(wRatio, pDifFinalCap)
    end

    -- Apply level correction to UL/LL
    -- https://www.ffxiah.com/forum/topic/57989/post-2016-level-correction-testing/
    -- Dice roll the 50/50 chance to select two different bounds. Mote has not yet implemented the spike by the time of this post so his ratio is not 50/50 rate.
    -- His model at the time and implemented spike, so the (0.0, 0.5) bounds also looks different
    -- https://www.bluegartr.com/threads/108161-pDif-and-damage?p=5007487&viewfull=1#post5007487
    local upperMax   = math.random(0, 1) == 0 and 0.5 or 0
    local upperBound = math.max(pDifUpperCap + levelDifFactor, upperMax)
    local lowerbound = math.max(pDifLowerCap + levelDifFactor, 0)

    if upperBound == 0 then
        return 0
    end

    pDif = math.random(lowerbound * 1000, upperBound * 1000) / 1000

    ----------------------------------------
    -- Step 4: Melee random factor.
    ----------------------------------------
    local meleeRandom = 1 + math.random(0, 5) * 0.01 -- 5 distinct values

    pDif = pDif * meleeRandom

    -- Crit damage bonus is a final modifier
    if isCritical then
        local critDamageBonus = utils.clamp(actor:getMod(xi.mod.CRIT_DMG_INCREASE) - target:getMod(xi.mod.CRIT_DEF_BONUS), 0, 100)
        pDif                  = pDif * (100 + critDamageBonus) / 100
    end

    return pDif
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
xi.combat.physical.calculateRangedPDIF = function(actor, target, weaponType, wsAttackMod, isCritical, applyLevelCorrection, tpIgnoresDefense, tpFactor, isWeaponskill, bonusRangedAttack)
    local pDif = 0

    ----------------------------------------
    -- Step 1: Attack / Defense Ratio
    ----------------------------------------
    local baseRatio       = 0
    local actorAttack     = 0
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

        if flourishEffect and flourishEffect:getPower() >= 2 then -- 2 or more Finishing Moves used.
            local meritCount = flourishEffect:getSubPower()

            flourishBonus = 1.25 + 0.01 * meritCount -- +1% attack bonus per merit -- TODO: do the merits apply even when FMs are < 2?
        end
    end

    -- TODO: it is unknown if ws attack mod and flourish bonus are additive or multiplicative
    actorAttack = math.max(1, math.floor((actor:getStat(xi.mod.RATT) + bonusRangedAttack - distancePenalty) * wsAttackMod * flourishBonus))

    -- Target Defense Modifiers.
    local ignoreDefenseFactor = 1

    if tpIgnoresDefense then
        ignoreDefenseFactor = 1 - tpFactor

        targetDefense = math.max(1, math.floor(targetDefense * ignoreDefenseFactor))
    end

    if targetDefense ~= 0 then
        baseRatio = actorAttack / targetDefense
    end

    ----------------------------------------
    -- Step 2: cRatio (Level correction, corrected ratio) Zone based!
    ----------------------------------------
    local levelDifFactor = 0

    -- Mod-based bypass for ranged level correction
    if actor:isPC() and actor:getMod(xi.mod.RA_IGNORE_LVL_DIFF) > 0 then
        applyLevelCorrection = false
    end

    -- https://forum.square-enix.com/ffxi/threads/31310-March-27-2013-(JST)-Version-Update
    -- TODO: There is some weirdness with needing 2 levels to start level correction in retail
    -- It is not currently implemented.
    if applyLevelCorrection then
        levelDifFactor = (actor:getMainLvl() - target:getMainLvl()) * (3 / 128) -- half the melee correction
    end

    -- Only players suffer from negative level difference.
    if
        not actor:isPC() and
        levelDifFactor < 0
    then
        levelDifFactor = 0
    end

    -- Players do not get positive level correction, only monsters
    if
        actor:isPC() and
        levelDifFactor > 0
    then
        levelDifFactor = 0
    end

    local cRatio = utils.clamp(baseRatio, 0, 10) -- Clamp for the lower limit, mainly.

    -- TODO: Presumably, pets get a Cap here if the target checks as 'Too Weak'. More info needed.

    ----------------------------------------
    -- Step 3: pDif Caps (Ranged)
    ----------------------------------------
    local pDifUpperCap       = 0
    local pDifLowerCap       = 0
    local damageLimitPlus    = actor:getMod(xi.mod.DAMAGE_LIMIT) / 100
    local damageLimitPercent = 1 + actor:getMod(xi.mod.DAMAGE_LIMITP) / 100
    local pDifFinalCap       = 0

    if actor:isPC() then
        pDifFinalCap = (xi.combat.physical.weaponCap(weaponType) + damageLimitPlus) * damageLimitPercent -- Added damage limit bonuses
    else
        -- 4.0 is guessed. there is some indication that mob pdif can go to 8.0 in ilvl content
        -- 3.0 with level correction matches player ranged pdif cap for 2013 and may need verification
        local basePDIF = applyLevelCorrection and 3 or 4
        pDifFinalCap   = (basePDIF + damageLimitPlus) * damageLimitPercent
    end

    pDif = utils.clamp(pDif, 0, pDifFinalCap)

    -- pDIF upper and lower caps.
    if cRatio < 0.9 then
        pDifUpperCap = cRatio * 10 / 9
        pDifLowerCap = cRatio
    elseif cRatio < 1.1 then
        pDifUpperCap = 1
        pDifLowerCap = 1
    else
        pDifUpperCap = math.min(cRatio, pDifFinalCap)
        pDifLowerCap = math.min(cRatio * 20 / 19 - 3 / 19, pDifFinalCap)
    end

    -- Add in level correction
    pDifUpperCap = pDifUpperCap + levelDifFactor
    pDifLowerCap = pDifLowerCap + levelDifFactor

    pDif = math.random(pDifLowerCap * 1000, pDifUpperCap * 1000) / 1000

    -- do not go negative, rolls below zero (proportionally) need to be rolled
    pDif = math.max(pDif, 0)

    ----------------------------------------
    -- Step 4: Ranged critical factor. Bypasses caps.
    ----------------------------------------
    if isCritical then
        pDif = pDif * 1.25
    end

    -- Step 5: TODO: True Shot.

    -- Crit damage bonus is a final modifier
    if isCritical then
        local critDamageBonus = utils.clamp(actor:getMod(xi.mod.CRIT_DMG_INCREASE) + actor:getMod(xi.mod.RANGED_CRIT_DMG_INCREASE) - target:getMod(xi.mod.CRIT_DEF_BONUS), 0, 100)
        pDif = pDif * (100 + critDamageBonus) / 100
    end

    return pDif
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
xi.combat.physical.calculateSwingCriticalRate = function(actor, target, actorTP, slot, optCritModTable)
    -- See reference at https://www.bg-wiki.com/ffxi/Critical_Hit_Rate
    local finalCriticalRate     = 0
    local baseCriticalRate      = 0.05
    local statBonus             = xi.combat.physical.criticalRateFromStatDiff(actor, target)
    local inninBonus            = xi.combat.physical.criticalRateFromInnin(actor, target)
    local fencerBonus           = xi.combat.physical.criticalRateFromFencer(actor)
    local buildingFlourishBonus = xi.combat.physical.criticalRateFromFlourish(actor)
    local weaponSlotBonus       = xi.combat.physical.criticalRateFromWeaponSlot(actor, slot)
    local modifierBonus         = actor:getMod(xi.mod.CRITHITRATE) / 100
    local meritBonus            = actor:getMerit(xi.merit.CRIT_HIT_RATE) / 100
    local targetCriticalEvasion = target:getMod(xi.mod.CRITICAL_HIT_EVASION) / 100
    local targetMeritPenalty    = target:getMerit(xi.merit.ENEMY_CRIT_RATE) / 100
    local tpFactor              = 0

    -- For weaponskills.
    if optCritModTable then
        tpFactor = xi.combat.physical.calculateTPfactor(actorTP, optCritModTable)
    end

    -- Add all different bonuses and clamp.
    finalCriticalRate = baseCriticalRate + statBonus + inninBonus + fencerBonus + buildingFlourishBonus + weaponSlotBonus + modifierBonus + meritBonus - targetCriticalEvasion - targetMeritPenalty + tpFactor

    return xi.combat.physical.clampCriticalRate(finalCriticalRate) -- TODO: Need confirmation of no upper cap.
end

---@param actor CBaseEntity
---@param target CBaseEntity
---@param actorTP number
---@param slot xi.slot
---@param optCritModTable table?
---@return integer
xi.combat.physical.calculateRangedCriticalRate = function(actor, target, actorTP, slot, optCritModTable)
    -- See reference at https://www.bg-wiki.com/ffxi/Critical_Hit_Rate
    local finalCriticalRate     = 0
    local baseCriticalRate      = 0.05
    local statBonus             = xi.combat.physical.criticalRateFromAGIDiff(actor, target)
    local inninBonus            = xi.combat.physical.criticalRateFromInnin(actor, target)
    local fencerBonus           = xi.combat.physical.criticalRateFromFencer(actor)
    local buildingFlourishBonus = xi.combat.physical.criticalRateFromFlourish(actor)
    local weaponSlotBonus       = xi.combat.physical.criticalRateFromWeaponSlot(actor, slot)
    local modifierBonus         = actor:getMod(xi.mod.CRITHITRATE) / 100
    local meritBonus            = actor:getMerit(xi.merit.CRIT_HIT_RATE) / 100
    local targetCriticalEvasion = target:getMod(xi.mod.CRITICAL_HIT_EVASION) / 100
    local targetMeritPenalty    = target:getMerit(xi.merit.ENEMY_CRIT_RATE) / 100
    local tpFactor              = 0

    -- For weaponskills.
    if optCritModTable then
        tpFactor = xi.combat.physical.calculateTPfactor(actorTP, optCritModTable)
    end

    -- Add all different bonuses and clamp.
    finalCriticalRate = baseCriticalRate + statBonus + inninBonus + fencerBonus + buildingFlourishBonus + weaponSlotBonus + modifierBonus + meritBonus - targetCriticalEvasion - targetMeritPenalty + tpFactor

    return xi.combat.physical.clampCriticalRate(finalCriticalRate) -- TODO: Need confirmation of no upper cap.
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

xi.combat.physical.canParry = function(defender, attacker)
    local canParry = false

    if
        defender:isFacing(attacker) and
        defender:isEngaged() and
        not defender:hasPreventActionEffect(true) -- Not stunned, slept, etc, but can parry when charmed
    then
        if defender:isPC() then
            if defender:getSkillRank(xi.skill.PARRY) == 0 then
                return false
            end

            local mainWeapon = defender:getEquippedItem(xi.slot.MAIN)

            if mainWeapon then
                canParry = mainWeapon:getSkillType() ~= xi.skill.HAND_TO_HAND
            end
        else
            canParry = defender:getMobMod(xi.mobMod.CAN_PARRY) > 0
        end
    end

    return canParry
end

-- Rough implementation of the following sheet, though we still dont know 100% how it all works
-- https://docs.google.com/spreadsheets/d/1wtS0d4nNqosMwFuHWb7fkCcj4naAvwhnPocf3qSGJjk/edit?gid=0#gid=0
xi.combat.physical.calculateParryRate = function(defender, attacker)
    local parryRate     = 0
    local attackerSkill = 0
    local defenderSkill = 0

    if defender:isPC() then
        defenderSkill = defender:getSkillLevel(xi.skill.PARRY) + defender:getMod(xi.mod.PARRY) + defender:getILvlParry()
    else
        defenderSkill = xi.data.skillLevel.getSkillCap(defender:getMainLvl(), xi.skillRank.A_PLUS)
    end

    if attacker:isPC() then
        attackerSkill = attacker:getSkillLevel(attacker:getWeaponSkillType(xi.slot.MAIN)) + attacker:getILvlSkill()
    else
        attackerSkill = xi.data.skillLevel.getSkillCap(attacker:getMainLvl(), xi.skillRank.A_PLUS)
    end

    local skillDelta = defenderSkill - attackerSkill

    if skillDelta <= 5 then -- between -10 and 5
    -- Matches the data
        -- X = 6
        -- but the slope is 36 / 9 for skill <= 5
        parryRate = math.floor(10 + (skillDelta - 6) / (36 / 9))

    else -- between 6 and 105
        -- In the formula "10 + (skill - X) / (60 / 9)"
        -- where X = 6, this matches the level 43 chigoe data exactly, and X = 11 matches the existing level 64 chigoe data
        -- We don't know what modifies X yet, (expansion, level, arbitrary?)
        -- Level 64 chigoe data is incomplete so we will use level 43 for now.
        parryRate = math.floor(10 + (skillDelta - 6) / (60 / 9))
    end

    parryRate = utils.clamp(parryRate, 5, 25)

    -- Issekigan grants parry rate bonus
    -- from best available data if you already capped out at 25% parry it grants another 25% bonus for ~50% parry rate
    if defender:hasStatusEffect(xi.effect.ISSEKIGAN) then
        parryRate = parryRate + defender:getStatusEffect(xi.effect.ISSEKIGAN):getPower()
    end

    -- Inquartata grants a flat parry rate bonus
    parryRate = parryRate + defender:getMod(xi.mod.INQUARTATA)

    return parryRate
end

xi.combat.physical.canGuard = function(defender, attacker)
    local canGuard = false

    -- per testing done by Genome guard can proc when petrified, stunned, or asleep
    -- https://genomeffxi.livejournal.com/18269.html
    if
        defender:isFacing(attacker) and
        defender:isEngaged() and
        not defender:hasPreventActionEffect(true) -- Not stunned, slept, etc, but can guard when charmed
    then
        if defender:isPC() and defender:getSkillRank(xi.skill.GUARD) > 0 then
            local mainWeapon = defender:getEquippedItem(xi.slot.MAIN)
            canGuard = (not mainWeapon) or mainWeapon:getSkillType() == xi.skill.HAND_TO_HAND
        elseif
            defender:isMob() or
            defender:isPet() or
            defender:isTrust()
        then
            canGuard = (defender:getMainJob() == xi.job.MNK or defender:getMainJob() == xi.job.PUP) and defender:getMobMod(xi.mobMod.CANNOT_GUARD) == 0
        end
    end

    return canGuard
end

xi.combat.physical.calculateGuardRate = function(defender, attacker)
    local guardRate     = 0
    local attackerSkill = 0
    local defenderSkill = 0

    if defender:isPC() then
        defenderSkill = defender:getSkillLevel(xi.skill.GUARD) + defender:getMod(xi.mod.GUARD) + defender:getILvlParry() -- getILvlParry also gets guard (h2h cannot have parry on the weapon)
    else
        defenderSkill = xi.data.skillLevel.getSkillCap(defender:getMainLvl(), xi.skillRank.A_PLUS)
    end

    if attacker:isPC() then
        attackerSkill = attacker:getSkillLevel(attacker:getWeaponSkillType(xi.slot.MAIN)) + attacker:getILvlSkill()
    else
        attackerSkill = xi.data.skillLevel.getSkillCap(attacker:getMainLvl(), xi.skillRank.A_PLUS)
    end

    local skillDelta = defenderSkill - attackerSkill

    -- This is approximated from parry
    -- Two data points showed that Guard was approximately 1% better, so skillDelta is at _least_ 6 lower on the same target
    -- The target was a lvl 43 chigoe, using the same parry skill vs guard skill with the known parrying rate data
    -- This is a placeholder and is likely more accurate than the previous code.
    if skillDelta <= 6 then
        guardRate = math.floor(10 + skillDelta / (36 / 9))
    else
        guardRate = math.floor(10 + skillDelta / (60 / 9))
    end

    -- Dodge's guard bonus goes over the cap
    guardRate = utils.clamp(guardRate, 5, 25) + defender:getMod(xi.mod.ADDITIVE_GUARD)

    return guardRate
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

        if xi.combat.physical.calculateParryRate(defender, attacker) * 100 >= math.random(1, 10000) then
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
        if xi.combat.physical.calculateGuardRate(defender, attacker) * 100 >= math.random(1, 10000) then
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
