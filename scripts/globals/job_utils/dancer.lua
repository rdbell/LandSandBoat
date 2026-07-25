-----------------------------------
-- Dancer Job Utilities
-- Dual-wired pure inject forms (slice 6735 / 0881):
--   waltz catalog/cost/amount/recast, finishing-move max/base/icon/clamp,
--   reverse/animated/building flourish products, step/flourish animations,
--   actionInfo, terpsichore allowlist, ability-check pure gates
-- Parity: internal/dancer
-----------------------------------
require('scripts/globals/jobpoints')
require('scripts/globals/magic')
require('scripts/globals/weaponskills')
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.dancer = xi.job_utils.dancer or {}
-----------------------------------

-----------------------------------
-- Pure inject pins (internal/dancer)
-----------------------------------
xi.job_utils.dancer.baseMaxFinishingMoves        = 5
xi.job_utils.dancer.finishingMoveDurationSec     = 7200
xi.job_utils.dancer.finishingMoveDurationMs      = 7200000
xi.job_utils.dancer.reverseFlourishMoveCap       = 5
xi.job_utils.dancer.buildingFlourishMaxPower     = 3
xi.job_utils.dancer.waltzPotencyCap              = 50
xi.job_utils.dancer.waltzCostPerMod              = 10
xi.job_utils.dancer.fanDanceMeritUnit            = 5
xi.job_utils.dancer.defaultCurePower             = 1.0
xi.job_utils.dancer.stepBaseAward                = 1
xi.job_utils.dancer.stepMainDNCAward             = 2
xi.job_utils.dancer.stepPrestoAward              = 5
xi.job_utils.dancer.animatedFlourishVEHigh       = 1500
xi.job_utils.dancer.animatedFlourishVELow        = 1000
xi.job_utils.dancer.animatedFlourishMovesHigh    = 2
xi.job_utils.dancer.animatedFlourishMovesLow     = 1
xi.job_utils.dancer.buildingFlourishDuration     = 60
xi.job_utils.dancer.prestoPower                  = 19
xi.job_utils.dancer.prestoDuration               = 30
xi.job_utils.dancer.prestoTick                   = 3
xi.job_utils.dancer.contradanceDuration          = 60
xi.job_utils.dancer.stepTPBase                   = 100
xi.job_utils.dancer.stepDebuffBaseStacks         = 1
xi.job_utils.dancer.stepDebuffBaseDuration       = 60
xi.job_utils.dancer.stepDebuffDurationAdd        = 30
xi.job_utils.dancer.stepDebuffDurationCapBase    = 120
xi.job_utils.dancer.stepDebuffMaxMain            = 10
xi.job_utils.dancer.stepDebuffMaxSub             = 5
xi.job_utils.dancer.prestoExtraStacks            = 4
xi.job_utils.dancer.stepHitRateBonus             = 10
xi.job_utils.dancer.tranceWaltzRecastCap         = 6
xi.job_utils.dancer.reverseFlourishBaseTP        = 95
xi.job_utils.dancer.reverseFlourishSquareBase    = 5
xi.job_utils.dancer.reverseFlourishMeritScale    = 30
xi.job_utils.dancer.msgUnableToUseJA2            = 88
xi.job_utils.dancer.msgCannotOnThatTarg          = 155
xi.job_utils.dancer.msgNotEnoughTP               = 192
xi.job_utils.dancer.msgNoFinishingMoves          = 524
xi.job_utils.dancer.msgRequiresCombat            = 525
xi.job_utils.dancer.msgNoFootRiseFull            = 561

-- Waltz catalog: [abilityId] = { tpCost, statMultiplier, baseHp }
-- Numeric keys match xi.jobAbility when enums available; pure tests use numbers.
xi.job_utils.dancer.waltzAbilities =
{
    -- ability IDs: CURING_WALTZ=190 ... DIVINE_WALTZ_II=262, CURING_WALTZ_V=311
    [190] = { 200, 0.25,  60 },
    [191] = { 350, 0.50, 130 },
    [192] = { 500, 0.75, 270 },
    [193] = { 650, 1.00, 450 },
    [311] = { 800, 1.25, 600 },
    [195] = { 400, 0.25,  60 },
    [262] = { 800, 0.75, 270 },
}

-- Keep enum-keyed aliases when xi.jobAbility is loaded (host path).
if xi.jobAbility then
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.CURING_WALTZ    ] = { 200, 0.25,  60 }
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.CURING_WALTZ_II ] = { 350, 0.50, 130 }
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.CURING_WALTZ_III] = { 500, 0.75, 270 }
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.CURING_WALTZ_IV ] = { 650, 1.00, 450 }
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.CURING_WALTZ_V  ] = { 800, 1.25, 600 }
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.DIVINE_WALTZ    ] = { 400, 0.25,  60 }
    xi.job_utils.dancer.waltzAbilities[xi.jobAbility.DIVINE_WALTZ_II ] = { 800, 0.75, 270 }
end

-- animationTable[skill] = { step, flourish }; skill 0..12
xi.job_utils.dancer.animationTable =
{
    [0]  = { 15, 25 }, -- NONE
    [1]  = { 15, 25 }, -- HAND_TO_HAND
    [2]  = { 16, 26 }, -- DAGGER
    [3]  = { 14, 24 }, -- SWORD
    [4]  = { 19, 29 }, -- GREAT_SWORD
    [5]  = { 16, 26 }, -- AXE
    [6]  = { 18, 28 }, -- GREAT_AXE
    [7]  = { 18, 28 }, -- SCYTHE
    [8]  = { 20, 30 }, -- POLEARM
    [9]  = { 21, 31 }, -- KATANA
    [10] = { 22, 32 }, -- GREAT_KATANA
    [11] = { 17, 27 }, -- CLUB
    [12] = { 23, 33 }, -- STAFF
}

if xi.skill then
    xi.job_utils.dancer.animationTable[xi.skill.NONE        ] = { 15, 25 }
    xi.job_utils.dancer.animationTable[xi.skill.HAND_TO_HAND] = { 15, 25 }
    xi.job_utils.dancer.animationTable[xi.skill.DAGGER      ] = { 16, 26 }
    xi.job_utils.dancer.animationTable[xi.skill.SWORD       ] = { 14, 24 }
    xi.job_utils.dancer.animationTable[xi.skill.GREAT_SWORD ] = { 19, 29 }
    xi.job_utils.dancer.animationTable[xi.skill.AXE         ] = { 16, 26 }
    xi.job_utils.dancer.animationTable[xi.skill.GREAT_AXE   ] = { 18, 28 }
    xi.job_utils.dancer.animationTable[xi.skill.SCYTHE      ] = { 18, 28 }
    xi.job_utils.dancer.animationTable[xi.skill.POLEARM     ] = { 20, 30 }
    xi.job_utils.dancer.animationTable[xi.skill.KATANA      ] = { 21, 31 }
    xi.job_utils.dancer.animationTable[xi.skill.GREAT_KATANA] = { 22, 32 }
    xi.job_utils.dancer.animationTable[xi.skill.CLUB        ] = { 17, 27 }
    xi.job_utils.dancer.animationTable[xi.skill.STAFF       ] = { 23, 33 }
end

-- Terpsichore main-hand item IDs (numeric for pure tests).
xi.job_utils.dancer.terpsichoreTable = set
{
    18989, 19078, 19098, 19630, 19728, 19837, 19966, 20557, 20558, 20584,
}

if xi.item then
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_75     ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_80     ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_85     ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_90     ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_95     ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_99     ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_99_II  ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_119    ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_119_II ] = true
    xi.job_utils.dancer.terpsichoreTable[xi.item.TERPSICHORE_119_III] = true
end

-- actionInfo: [abilityId] = { miss, hit }
xi.job_utils.dancer.actionInfo =
{
    [201] = { 1, 5 }, -- QUICKSTEP
    [202] = { 2, 6 }, -- BOX_STEP
    [203] = { 3, 7 }, -- STUTTER_STEP
    [312] = { 4, 8 }, -- FEATHER_STEP
    [209] = { 1, 5 }, -- WILD_FLOURISH
    [205] = { 2, 6 }, -- DESPERATE_FLOURISH
    [207] = { 3, 7 }, -- VIOLENT_FLOURISH
}

if xi.jobAbility then
    xi.job_utils.dancer.actionInfo[xi.jobAbility.QUICKSTEP         ] = { 1, 5 }
    xi.job_utils.dancer.actionInfo[xi.jobAbility.BOX_STEP          ] = { 2, 6 }
    xi.job_utils.dancer.actionInfo[xi.jobAbility.STUTTER_STEP      ] = { 3, 7 }
    xi.job_utils.dancer.actionInfo[xi.jobAbility.FEATHER_STEP      ] = { 4, 8 }
    xi.job_utils.dancer.actionInfo[xi.jobAbility.WILD_FLOURISH     ] = { 1, 5 }
    xi.job_utils.dancer.actionInfo[xi.jobAbility.DESPERATE_FLOURISH] = { 2, 6 }
    xi.job_utils.dancer.actionInfo[xi.jobAbility.VIOLENT_FLOURISH  ] = { 3, 7 }
end

-----------------------------------
-- Pure inject APIs
-----------------------------------

-- Pure: lookup waltz catalog row. returns row or nil
xi.job_utils.dancer.lookupWaltz = function(abilityId)
    return xi.job_utils.dancer.waltzAbilities[abilityId]
end

-- Pure: is Divine Waltz / II
xi.job_utils.dancer.isDivineWaltz = function(abilityId)
    if xi.jobAbility then
        return abilityId == xi.jobAbility.DIVINE_WALTZ or abilityId == xi.jobAbility.DIVINE_WALTZ_II
    end

    return abilityId == 195 or abilityId == 262
end

-- Pure: WaltzCost(baseCost, waltzCostMod)
xi.job_utils.dancer.waltzCostFromParams = function(params)
    params = params or {}
    return (params.baseCost or 0) - (params.waltzCostMod or 0) * xi.job_utils.dancer.waltzCostPerMod
end

-- Pure: catalog lookup + cost
-- returns: cost, ok
xi.job_utils.dancer.waltzCostForAbilityFromParams = function(params)
    params = params or {}
    local row = xi.job_utils.dancer.lookupWaltz(params.abilityId)
    if not row then
        return 0, false
    end

    return xi.job_utils.dancer.waltzCostFromParams({
        baseCost      = row[1],
        waltzCostMod  = params.waltzCostMod,
    }), true
end

-- Pure: WaltzAmount heal product
-- params: abilityId, mainJobDNC, targetVIT, casterCHR, waltzPotency,
--         hasContradance, curePower, targetMissingHP
-- returns: amount, ok
xi.job_utils.dancer.waltzAmountFromParams = function(params)
    params = params or {}
    local row = xi.job_utils.dancer.lookupWaltz(params.abilityId)
    if not row then
        return 0, false
    end

    local mult = row[2]
    if not params.mainJobDNC then
        mult = mult / 2
    end

    local amt = ((params.targetVIT or 0) + (params.casterCHR or 0)) * mult + row[3]

    local potency = params.waltzPotency or 0
    if potency > xi.job_utils.dancer.waltzPotencyCap then
        potency = xi.job_utils.dancer.waltzPotencyCap
    end

    amt = math.floor(amt * (1.0 + potency / 100.0))

    if params.hasContradance then
        amt = amt * 2
    end

    local curePower = params.curePower
    if not curePower or curePower == 0 then
        curePower = xi.job_utils.dancer.defaultCurePower
    end

    amt = amt * curePower

    local missing = params.targetMissingHP or 0
    if missing < 0 then
        missing = 0
    end

    if amt > missing then
        amt = missing
    end

    return amt, true
end

-- Pure: MaxFinishingMoves
xi.job_utils.dancer.maxFinishingMovesFromParams = function(params)
    params = params or {}
    return xi.job_utils.dancer.baseMaxFinishingMoves + (params.bonus or 0)
end

-- Pure: StepFinishingMovesBase
-- params: hasPresto, mainJobDNC, mainHandIsTerps, stepFinishMod
xi.job_utils.dancer.stepFinishingMovesBaseFromParams = function(params)
    params = params or {}
    local num = xi.job_utils.dancer.stepBaseAward
    if params.hasPresto then
        num = xi.job_utils.dancer.stepPrestoAward
    elseif params.mainJobDNC then
        num = xi.job_utils.dancer.stepMainDNCAward
    end

    if params.mainHandIsTerps then
        num = num + (params.stepFinishMod or 0)
    end

    return num
end

-- Pure: FinishingMoveIcon (returns nil when < 1 for host parity with Lua nil)
xi.job_utils.dancer.finishingMoveIconFromParams = function(numMoves)
    if not numMoves or numMoves < 1 then
        return nil
    end

    local effectFM1 = 381
    if xi.effect and xi.effect.FINISHING_MOVE_1 then
        effectFM1 = xi.effect.FINISHING_MOVE_1
    end

    local effectFM6 = 588
    if xi.effect and xi.effect.FINISHING_MOVE_6 then
        effectFM6 = xi.effect.FINISHING_MOVE_6
    end

    if numMoves <= 5 then
        return effectFM1 + numMoves - 1
    end

    return effectFM6
end

-- Pure: ClampFinishingMoves
xi.job_utils.dancer.clampFinishingMovesFromParams = function(params)
    params = params or {}
    local numMoves = params.numMoves or 0
    local max      = params.max or 0
    if max < 0 then
        max = 0
    end

    if numMoves < 0 then
        return 0
    end

    if numMoves > max then
        return max
    end

    return numMoves
end

-- Pure: ReverseFlourishTP
-- params: numMoves, flourishIIJP, reverseFlourishGear, reverseFlourishMerits
-- returns: tp, usedMoves
xi.job_utils.dancer.reverseFlourishTPFromParams = function(params)
    params = params or {}
    local used = params.numMoves or 0
    if used > xi.job_utils.dancer.reverseFlourishMoveCap then
        used = xi.job_utils.dancer.reverseFlourishMoveCap
    end

    if used < 0 then
        used = 0
    end

    local tp = (xi.job_utils.dancer.reverseFlourishBaseTP + (params.flourishIIJP or 0)) * used
        + (xi.job_utils.dancer.reverseFlourishSquareBase + (params.reverseFlourishGear or 0)) * used * used
        + xi.job_utils.dancer.reverseFlourishMeritScale * (params.reverseFlourishMerits or 0)

    return tp, used
end

-- Pure: BuildingFlourishPower
xi.job_utils.dancer.buildingFlourishPowerFromParams = function(availableMoves)
    availableMoves = availableMoves or 0
    if availableMoves < 0 then
        return 0
    end

    if availableMoves > xi.job_utils.dancer.buildingFlourishMaxPower then
        return xi.job_utils.dancer.buildingFlourishMaxPower
    end

    return availableMoves
end

-- Pure: AnimatedFlourish VE + moves
-- returns: { ve, usedMoves }
xi.job_utils.dancer.animatedFlourishFromParams = function(numMoves)
    numMoves = numMoves or 0
    if numMoves >= 2 then
        return {
            ve        = xi.job_utils.dancer.animatedFlourishVEHigh,
            usedMoves = xi.job_utils.dancer.animatedFlourishMovesHigh,
        }
    end

    return {
        ve        = xi.job_utils.dancer.animatedFlourishVELow,
        usedMoves = xi.job_utils.dancer.animatedFlourishMovesLow,
    }
end

-- Pure: StepAnimation
xi.job_utils.dancer.stepAnimationFromParams = function(weaponSkillType)
    weaponSkillType = weaponSkillType or 0
    if weaponSkillType <= 12 then
        local row = xi.job_utils.dancer.animationTable[weaponSkillType]
        if row then
            return row[1]
        end
    end

    return 0
end

-- Pure: FlourishAnimation
xi.job_utils.dancer.flourishAnimationFromParams = function(weaponSkillType)
    weaponSkillType = weaponSkillType or 0
    if weaponSkillType <= 12 then
        local row = xi.job_utils.dancer.animationTable[weaponSkillType]
        if row then
            return row[2]
        end
    end

    return 0
end

-- Pure: LookupActionInfo — returns miss, hit, ok
xi.job_utils.dancer.lookupActionInfoFromParams = function(abilityId)
    local row = xi.job_utils.dancer.actionInfo[abilityId]
    if not row then
        return 0, 0, false
    end

    return row[1], row[2], true
end

-- Pure: IsTerpsichore
xi.job_utils.dancer.isTerpsichore = function(itemId)
    return xi.job_utils.dancer.terpsichoreTable[itemId] == true
end

-- Pure: WaltzRecast
-- params: baseRecast, waltzDelay, hasFanDance, fanDanceMeritValue
xi.job_utils.dancer.waltzRecastFromParams = function(params)
    params = params or {}
    local newRecast = (params.baseRecast or 0) + (params.waltzDelay or 0)
    if
        params.hasFanDance and
        (params.fanDanceMeritValue or 0) > xi.job_utils.dancer.fanDanceMeritUnit
    then
        newRecast = newRecast * (105 - (params.fanDanceMeritValue or 0)) / 100
    end

    if newRecast < 0 then
        return 0
    end

    return newRecast
end

-- Pure check: Step combat + TP/Trance
-- params: inCombat, hasTrance, playerTP, stepTPCost (default 100)
-- returns: msg, ok
xi.job_utils.dancer.checkStepAbilityFromParams = function(params)
    params = params or {}
    if not params.inCombat then
        return xi.job_utils.dancer.msgRequiresCombat, false
    end

    if params.hasTrance then
        return 0, true
    end

    local cost = params.stepTPCost or xi.job_utils.dancer.stepTPBase
    if (params.playerTP or 0) < cost then
        return xi.job_utils.dancer.msgNotEnoughTP, false
    end

    return 0, true
end

-- Pure check: No Foot Rise (FM already at max)
-- params: hasFMEffect, currentFM, maxFM
-- returns: msg, ok
xi.job_utils.dancer.checkNoFootRiseAbilityFromParams = function(params)
    params = params or {}
    if params.hasFMEffect and (params.currentFM or 0) >= (params.maxFM or 0) then
        return xi.job_utils.dancer.msgNoFootRiseFull, false
    end

    return 0, true
end

-- Pure check: Flourish combat + min FM
-- params: combatOnly, inCombat, numFinishingMoves, minimumCost
-- returns: msg, ok
xi.job_utils.dancer.checkFlourishAbilityFromParams = function(params)
    params = params or {}
    if params.combatOnly and not params.inCombat then
        return xi.job_utils.dancer.msgRequiresCombat, false
    end

    if (params.numFinishingMoves or 0) >= (params.minimumCost or 0) then
        return 0, true
    end

    return xi.job_utils.dancer.msgNoFinishingMoves, false
end

-----------------------------------
-- Local host helpers (inject pure)
-----------------------------------
local function getMaxFinishingMoves(player)
    return xi.job_utils.dancer.maxFinishingMovesFromParams({
        bonus = player:getMod(xi.mod.MAX_FINISHING_MOVE_BONUS),
    })
end

local function getStepFinishingMovesBase(player)
    local mainHandWeapon = player:getEquipID(xi.slot.MAIN)
    return xi.job_utils.dancer.stepFinishingMovesBaseFromParams({
        hasPresto       = player:hasStatusEffect(xi.effect.PRESTO),
        mainJobDNC      = player:getMainJob() == xi.job.DNC,
        mainHandIsTerps = xi.job_utils.dancer.isTerpsichore(mainHandWeapon),
        stepFinishMod   = player:getMod(xi.mod.STEP_FINISH),
    })
end

local function getFinishingMoveIcon(numMoves)
    return xi.job_utils.dancer.finishingMoveIconFromParams(numMoves)
end

local function setFinishingMoves(player, numMoves)
    local finishingEffect = player:getStatusEffect(xi.effect.FINISHING_MOVE_1)
    numMoves = xi.job_utils.dancer.clampFinishingMovesFromParams({
        numMoves = numMoves,
        max      = getMaxFinishingMoves(player),
    })

    if finishingEffect then
        if numMoves == 0 then
            player:delStatusEffect(xi.effect.FINISHING_MOVE_1)
        else
            finishingEffect:setPower(numMoves)
            finishingEffect:setIcon(getFinishingMoveIcon(numMoves))
            finishingEffect:setDuration(2 * 60 * 60 * 1000)
        end
    else
        player:addStatusEffect(xi.effect.FINISHING_MOVE_1, {
            power    = numMoves,
            duration = 7200,
            origin   = player,
            icon     = getFinishingMoveIcon(numMoves),
        })
    end
end

local function getStepAnimation(weaponSkillType)
    return xi.job_utils.dancer.stepAnimationFromParams(weaponSkillType)
end

local function getFlourishAnimation(weaponSkillType)
    return xi.job_utils.dancer.flourishAnimationFromParams(weaponSkillType)
end

-----------------------------------
-- Ability Check (host → pure)
-----------------------------------
xi.job_utils.dancer.checkStepAbility = function(player, target, ability)
    local msg, ok = xi.job_utils.dancer.checkStepAbilityFromParams({
        inCombat  = player:getAnimation() == 1,
        hasTrance = player:hasStatusEffect(xi.effect.TRANCE),
        playerTP  = player:getTP(),
        stepTPCost = xi.job_utils.dancer.stepTPBase,
    })
    if ok then
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.dancer.checkNoFootRiseAbility = function(player, target, ability)
    local fmEffect = player:getStatusEffect(xi.effect.FINISHING_MOVE_1)
    local current  = 0
    if fmEffect then
        current = fmEffect:getPower()
    end

    local msg, ok = xi.job_utils.dancer.checkNoFootRiseAbilityFromParams({
        hasFMEffect = fmEffect ~= nil,
        currentFM   = current,
        maxFM       = getMaxFinishingMoves(player),
    })
    if ok then
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.dancer.checkFlourishAbility = function(player, target, ability, combatOnly, minimumCost)
    local numFinishingMoves = 0
    local flourishEffect = player:getStatusEffect(xi.effect.FINISHING_MOVE_1)
    if flourishEffect then
        numFinishingMoves = flourishEffect:getPower()
    end

    local msg, ok = xi.job_utils.dancer.checkFlourishAbilityFromParams({
        combatOnly         = combatOnly,
        inCombat           = player:getAnimation() == 1,
        numFinishingMoves  = numFinishingMoves,
        minimumCost        = minimumCost,
    })
    if ok then
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.dancer.checkWaltzAbility = function(player, target, ability)
    local abilityId = ability:getID()
    local waltzCost, costOk = xi.job_utils.dancer.waltzCostForAbilityFromParams({
        abilityId    = abilityId,
        waltzCostMod = player:getMod(xi.mod.WALTZ_COST),
    })
    if not costOk then
        -- Unknown ability ID — fall through with zero cost check path
        waltzCost = 0
    end

    if target:getHP() == 0 then
        return xi.msg.basic.CANNOT_ON_THAT_TARG, 0
    elseif player:hasStatusEffect(xi.effect.SABER_DANCE) then
        return xi.msg.basic.UNABLE_TO_USE_JA2, 0
    elseif player:hasStatusEffect(xi.effect.TRANCE) then
        ability:setRecast(math.min(ability:getRecast(), xi.job_utils.dancer.tranceWaltzRecastCap))
        ability:setPostActionCleanupEffect(xi.effect.CONTRADANCE)
        return 0, 0
    elseif player:getTP() < waltzCost then
        return xi.msg.basic.NOT_ENOUGH_TP, 0
    else
        local newRecast = xi.job_utils.dancer.waltzRecastFromParams({
            baseRecast         = ability:getRecast(),
            waltzDelay         = player:getMod(xi.mod.WALTZ_DELAY),
            hasFanDance        = player:hasStatusEffect(xi.effect.FAN_DANCE),
            fanDanceMeritValue = player:getMerit(xi.merit.FAN_DANCE),
        })

        ability:setRecast(utils.clamp(newRecast, 0, newRecast))
        ability:setPostActionCleanupEffect(xi.effect.CONTRADANCE)

        return 0, 0
    end
end

-----------------------------------
-- Ability Use (host → pure)
-----------------------------------
xi.job_utils.dancer.useStepAbility = function(player, target, ability, action, stepEffect)
    local missInfo, hitInfo = xi.job_utils.dancer.lookupActionInfoFromParams(ability:getID())
    local infoValue        = missInfo
    local stepDurationGift = player:getJobPointLevel(xi.jp.STEP_DURATION)
    local debuffStacks     = xi.job_utils.dancer.stepDebuffBaseStacks
    local debuffDuration   = xi.job_utils.dancer.stepDebuffBaseDuration + stepDurationGift
    local hitRate          = xi.combat.physicalHitRate.getPhysicalHitRate(
        player,
        target,
        xi.job_utils.dancer.stepHitRateBonus + player:getMod(xi.mod.STEP_ACCURACY),
        xi.attackAnimation.RIGHT_ATTACK,
        false
    )

    if not player:hasStatusEffect(xi.effect.TRANCE) then
        player:delTP(xi.job_utils.dancer.stepTPBase + player:getMod(xi.mod.STEP_TP_CONSUMED))
    end

    if math.random() <= hitRate then
        local maxSteps         = player:getMainJob() == xi.job.DNC
            and xi.job_utils.dancer.stepDebuffMaxMain
            or xi.job_utils.dancer.stepDebuffMaxSub
        local debuffEffect     = target:getStatusEffect(stepEffect)
        local origDebuffStacks = 0
        infoValue              = hitInfo

        local fmEffect   = player:getStatusEffect(xi.effect.FINISHING_MOVE_1)
        local addedMoves = getStepFinishingMovesBase(player)

        if fmEffect then
            addedMoves = addedMoves + fmEffect:getPower()
        end

        setFinishingMoves(player, math.min(addedMoves, getMaxFinishingMoves(player)))

        if player:hasStatusEffect(xi.effect.PRESTO) then
            debuffStacks = debuffStacks + xi.job_utils.dancer.prestoExtraStacks
            player:delStatusEffect(xi.effect.PRESTO)
        end

        if debuffEffect then
            origDebuffStacks = debuffEffect:getPower()
            debuffStacks     = debuffStacks + origDebuffStacks
            debuffDuration   = debuffEffect:getDuration()

            debuffStacks   = math.min(debuffStacks, maxSteps)
            debuffDuration = math.min(
                debuffEffect:getDuration() + xi.job_utils.dancer.stepDebuffDurationAdd + stepDurationGift,
                xi.job_utils.dancer.stepDebuffDurationCapBase + stepDurationGift
            )

            if maxSteps >= origDebuffStacks then
                target:delStatusEffectSilent(stepEffect)
            end
        end

        if maxSteps >= origDebuffStacks then
            target:addStatusEffect(stepEffect, {
                power    = debuffStacks,
                duration = debuffDuration,
                origin   = player,
            })
        else
            ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
        end
    else
        ability:setMsg(xi.msg.basic.JA_MISS)
    end

    action:setAnimation(target:getID(), getStepAnimation(player:getWeaponSkillType(xi.slot.MAIN)))

    if player:getObjType() == xi.objType.TRUST then
        local name = string.lower(player:getName())
        if name == 'uka_totlihn' or name == 'mumor' or name == 'mumor_ii' then
            action:setAnimation(target:getID(), getStepAnimation(xi.skill.CLUB))
        elseif name == 'mayakov' then
            action:setAnimation(target:getID(), getStepAnimation(xi.skill.SWORD))
        end
    end

    action:info(target:getID(), infoValue)

    return debuffStacks
end

xi.job_utils.dancer.usePrestoAbility = function(player, target, ability, action)
    target:addStatusEffect(xi.effect.PRESTO, {
        power    = xi.job_utils.dancer.prestoPower,
        duration = xi.job_utils.dancer.prestoDuration,
        origin   = player,
        tick     = xi.job_utils.dancer.prestoTick,
    })

    return xi.effect.PRESTO
end

xi.job_utils.dancer.useNoFootRiseAbility = function(player, target, ability, action)
    local addedMoves = player:getMerit(xi.merit.NO_FOOT_RISE)
    local fmEffect   = player:getStatusEffect(xi.effect.FINISHING_MOVE_1)

    if fmEffect then
        addedMoves = addedMoves + fmEffect:getPower()
    end

    addedMoves = math.min(addedMoves, getMaxFinishingMoves(player))
    setFinishingMoves(player, addedMoves)

    return addedMoves
end

xi.job_utils.dancer.useReverseFlourishAbility = function(player, target, ability, action)
    local numMoves = player:getStatusEffect(xi.effect.FINISHING_MOVE_1):getPower()
    local tpGained, usedMoves = xi.job_utils.dancer.reverseFlourishTPFromParams({
        numMoves              = numMoves,
        flourishIIJP          = player:getJobPointLevel(xi.jp.FLOURISH_II_EFFECT),
        reverseFlourishGear   = player:getMod(xi.mod.REVERSE_FLOURISH_EFFECT),
        reverseFlourishMerits = player:getMerit(xi.merit.REVERSE_FLOURISH_EFFECT),
    })

    player:addTP(tpGained)
    setFinishingMoves(player, numMoves - usedMoves)

    return tpGained
end

xi.job_utils.dancer.useAnimatedFlourishAbility = function(player, target, ability, action)
    local jpBonusVE = player:getJobPointLevel(xi.jp.FLOURISH_I_EFFECT) * 10
    local numMoves  = player:getStatusEffect(xi.effect.FINISHING_MOVE_1):getPower()
    local plan      = xi.job_utils.dancer.animatedFlourishFromParams(numMoves)

    target:addEnmity(player, 0, plan.ve + jpBonusVE)
    setFinishingMoves(player, numMoves - plan.usedMoves)
end

xi.job_utils.dancer.useDesperateFlourishAbility = function(player, target, ability, action)
    local numMoves  = player:getStatusEffect(xi.effect.FINISHING_MOVE_1):getPower()
    local missInfo, hitInfo = xi.job_utils.dancer.lookupActionInfoFromParams(ability:getID())
    local infoValue = missInfo

    setFinishingMoves(player, numMoves - 1)

    if
        math.random() <= xi.weaponskills.getHitRate(player, target, player:getJobPointLevel(xi.jp.FLOURISH_I_EFFECT), xi.attackAnimation.LEFT_ATTACK) or
        (player:hasStatusEffect(xi.effect.SNEAK_ATTACK) and player:isBehind(target))
    then
        infoValue = hitInfo
        local resistRate = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, xi.skillRank.A_PLUS, xi.element.WIND, xi.mod.INT, xi.effect.WEIGHT, 0)

        if
            not xi.data.statusEffect.isTargetImmune(target, xi.effect.WEIGHT, xi.element.WIND) and
            not xi.data.statusEffect.isTargetResistant(player, target, xi.effect.WEIGHT) and
            not xi.data.statusEffect.isEffectNullified(target, xi.effect.WEIGHT, 0) and
            resistRate > 0.25 and
            target:addStatusEffect(xi.effect.WEIGHT, { power = 50, duration = 60 * resistRate, origin = player })
        then
            ability:setMsg(xi.msg.basic.JA_ENFEEB_IS)
        else
            ability:setMsg(xi.msg.basic.JA_DAMAGE)
        end

        action:setAnimation(target:getID(), getFlourishAnimation(player:getWeaponSkillType(xi.slot.MAIN)))
        action:info(target:getID(), infoValue)

        return xi.effect.WEIGHT
    else
        ability:setMsg(xi.msg.basic.JA_MISS)
        action:info(target:getID(), infoValue)

        return 0
    end
end

-- TODO: This ability needs verification
xi.job_utils.dancer.useViolentFlourishAbility = function(player, target, ability, action)
    local numMoves  = player:getStatusEffect(xi.effect.FINISHING_MOVE_1):getPower()
    local hitRate   = xi.combat.physicalHitRate.getPhysicalHitRate(player, target, 100, xi.attackAnimation.RIGHT_ATTACK, false)
    local missInfo, hitInfo = xi.job_utils.dancer.lookupActionInfoFromParams(ability:getID())
    local infoValue = missInfo
    setFinishingMoves(player, numMoves - 1)

    if
        math.random() <= hitRate or
        (player:hasStatusEffect(xi.effect.SNEAK_ATTACK) and player:isBehind(target))
    then
        infoValue          = hitInfo
        local weaponDamage = player:getWeaponDmg()
        local weaponType   = player:getWeaponSkillType(xi.slot.MAIN)
        if player:getWeaponSkillType(xi.slot.MAIN) == xi.skill.HAND_TO_HAND then
            local h2hSkill = player:getSkillLevel(xi.skill.HAND_TO_HAND) * 0.11 + 3

            weaponDamage = weaponDamage - 3 + h2hSkill
        end

        local applyLevelCorrection = xi.data.levelCorrection.isLevelCorrectedZone(player)
        local baseDmg              = weaponDamage + xi.combat.physical.calculateMeleeStatFactor(player, target)
        local pdif                 = xi.combat.physical.calculateMeleePDIF(player, target, weaponType, 1.0, false, applyLevelCorrection, false, 0.0, false, xi.slot.MAIN, false)
        local dmg                  = baseDmg * pdif

        dmg = utils.handleStoneskin(target, dmg)
        target:takeDamage(dmg, player, xi.attackType.PHYSICAL, player:getWeaponDamageType(xi.slot.MAIN))
        target:updateEnmityFromDamage(player, dmg)
        action:recordDamage(target, xi.attackType.PHYSICAL, dmg)

        local bonusMacc  = player:getMod(xi.mod.VFLOURISH_MACC)
        local resistRate = xi.combat.magicHitRate.calculateResistRate(player, target, 0, 0, xi.skillRank.A_PLUS, xi.element.THUNDER, xi.mod.INT, xi.effect.STUN, bonusMacc)

        if
            not xi.data.statusEffect.isTargetImmune(target, xi.effect.STUN, xi.element.THUNDER) and
            not xi.data.statusEffect.isTargetResistant(player, target, xi.effect.STUN) and
            not xi.data.statusEffect.isEffectNullified(target, xi.effect.STUN, 0) and
            xi.data.statusEffect.isResistRateSuccessfull(xi.effect.STUN, resistRate, 0)
        then
            target:addStatusEffect(xi.effect.STUN, { power = 1, duration = 2, origin = player })
        else
            ability:setMsg(xi.msg.basic.JA_DAMAGE)
        end

        action:setAnimation(target:getID(), getFlourishAnimation(player:getWeaponSkillType(xi.slot.MAIN)))
        action:info(target:getID(), infoValue)

        return dmg
    else
        ability:setMsg(xi.msg.basic.JA_MISS)
        action:info(target:getID(), infoValue)

        return 0
    end
end

xi.job_utils.dancer.useBuildingFlourishAbility = function(player, target, ability)
    local flourishMerits = player:getMerit(xi.merit.BUILDING_FLOURISH_EFFECT)
    local availableMoves = player:getStatusEffect(xi.effect.FINISHING_MOVE_1):getPower()
    local power          = xi.job_utils.dancer.buildingFlourishPowerFromParams(availableMoves)

    player:addStatusEffect(xi.effect.BUILDING_FLOURISH, {
        power    = power,
        duration = xi.job_utils.dancer.buildingFlourishDuration,
        origin   = player,
        subPower = flourishMerits,
    })
    setFinishingMoves(player, availableMoves - power)
end

xi.job_utils.dancer.useWildFlourishAbility = function(player, target, ability, action)
    local numMoves  = player:getStatusEffect(xi.effect.FINISHING_MOVE_1):getPower()
    local missInfo, hitInfo = xi.job_utils.dancer.lookupActionInfoFromParams(ability:getID())
    local infoValue = missInfo

    -- TODO: Wild Flourish can miss
    if
        not target:hasStatusEffect(xi.effect.CHAINBOUND, 0) and
        not target:hasStatusEffect(xi.effect.SKILLCHAIN, 0)
    then
        infoValue = hitInfo
        target:addStatusEffect(xi.effect.CHAINBOUND, {
            power    = 1,
            duration = 10,
            origin   = player,
            icon     = 0,
            subPower = 1,
        })
    else
        ability:setMsg(xi.msg.basic.JA_NO_EFFECT)
    end

    action:setAnimation(target:getID(), getFlourishAnimation(player:getWeaponSkillType(xi.slot.MAIN)))
    action:info(target:getID(), infoValue)
    setFinishingMoves(player, numMoves - 2)

    return 0
end

xi.job_utils.dancer.useContradanceAbility = function(player, target, ability)
    player:addStatusEffect(xi.effect.CONTRADANCE, {
        duration = xi.job_utils.dancer.contradanceDuration,
        origin   = player,
    })

    return xi.effect.CONTRADANCE
end

xi.job_utils.dancer.useWaltzAbility = function(player, target, ability, action)
    local abilityId = ability:getID()
    local waltzCost, costOk = xi.job_utils.dancer.waltzCostForAbilityFromParams({
        abilityId    = abilityId,
        waltzCostMod = player:getMod(xi.mod.WALTZ_COST),
    })
    if not costOk then
        waltzCost = 0
    end

    -- Handle TP cost.
    if not player:hasStatusEffect(xi.effect.TRANCE) then
        if xi.job_utils.dancer.isDivineWaltz(abilityId) then
            if player:getID() == target:getID() then
                player:delTP(waltzCost)
            end
        else
            player:delTP(waltzCost)
        end
    end

    local amtCured, ok = xi.job_utils.dancer.waltzAmountFromParams({
        abilityId       = abilityId,
        mainJobDNC      = player:getMainJob() == xi.job.DNC,
        targetVIT       = target:getStat(xi.mod.VIT),
        casterCHR       = player:getStat(xi.mod.CHR),
        waltzPotency    = player:getMod(xi.mod.WALTZ_POTENCY),
        hasContradance  = player:hasStatusEffect(xi.effect.CONTRADANCE),
        curePower       = xi.settings.main.CURE_POWER,
        targetMissingHP = target:getMaxHP() - target:getHP(),
    })
    if not ok then
        amtCured = 0
    end

    target:restoreHP(amtCured)
    target:wakeUp()
    player:updateEnmityFromCure(target, amtCured)

    return amtCured
end
