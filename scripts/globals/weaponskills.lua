-----------------------------------
-- Contains all common weaponskill calculations including but not limited to:
-- fSTR
-- Alpha
-- Ratio -> cRatio
-- min/max cRatio
-- applications of fTP
-- applications of critical hits ('Critical hit rate varies with TP.')
-- applications of accuracy mods ('Accuracy varies with TP.')
-- applications of damage mods ('Damage varies with TP.')
-- performance of the actual WS (rand numbers, etc)
-----------------------------------
require('scripts/globals/magicburst')
require('scripts/globals/ability')
require('scripts/globals/magic')
require('scripts/globals/combat/physical_utilities')
-----------------------------------
xi = xi or {}
xi.weaponskills = xi.weaponskills or {}

-----------------------------------
-- Pure: weaponskill single-hit shadow absorb (slice 6752 / internal/shadowabsorb.SingleHitAbsorb)
-- Unlike utils.shadowAbsorb: Blink 80% proc when Utsusemi 0; no COPY_IMAGE_4 for remaining >= 4;
-- remaining 0 deletes both COPY_IMAGE and BLINK.
-- params: utsusemi, blink, blinkProcRoll, hasCopyImageEffect
-- returns: absorbed, remaining, usedUtsusemi, usedBlink, icon, setIcon, delCopyImage, delBlink
-----------------------------------
xi.weaponskills.wsBlinkProcThreshold = 80

xi.weaponskills.singleHitShadowAbsorbFromParams = function(params)
    params = params or {}
    local utsusemi = params.utsusemi or 0
    local blink = params.blink or 0
    local blinkProcRoll = params.blinkProcRoll or 0
    local hasCopyImageEffect = params.hasCopyImageEffect

    local shadows = utsusemi
    local usedUtsu = false
    local usedBlink = false

    if shadows == 0 then
        if blinkProcRoll <= xi.weaponskills.wsBlinkProcThreshold then
            shadows = blink
            if shadows > 0 then
                usedBlink = true
            end
        end
    else
        usedUtsu = true
    end

    if shadows <= 0 then
        return {
            absorbed     = false,
            remaining    = 0,
            usedUtsusemi = false,
            usedBlink    = false,
            icon         = 0,
            setIcon      = false,
            delCopyImage = false,
            delBlink     = false,
        }
    end

    local remaining = shadows - 1
    local res = {
        absorbed     = true,
        remaining    = remaining,
        usedUtsusemi = usedUtsu,
        usedBlink    = usedBlink,
        icon         = 0,
        setIcon      = false,
        delCopyImage = false,
        delBlink     = false,
    }

    if remaining == 0 then
        res.delCopyImage = true
        res.delBlink = true
        return res
    end

    if usedUtsu then
        -- WS local only maps remaining 1/2/3 (no COPY_IMAGE_4 branch).
        if remaining == 1 then
            res.icon = xi.effect.COPY_IMAGE
        elseif remaining == 2 then
            res.icon = xi.effect.COPY_IMAGE_2
        elseif remaining == 3 then
            res.icon = xi.effect.COPY_IMAGE_3
        end

        res.setIcon = hasCopyImageEffect and res.icon ~= 0
    end

    return res
end

-- Entity host: inject mods/RNG → pure → setMod/setIcon/delStatusEffect.
local function shadowAbsorb(target)
    local res = xi.weaponskills.singleHitShadowAbsorbFromParams({
        utsusemi           = target:getMod(xi.mod.UTSUSEMI),
        blink              = target:getMod(xi.mod.BLINK),
        blinkProcRoll      = math.random(1, 100),
        hasCopyImageEffect = target:getStatusEffect(xi.effect.COPY_IMAGE) ~= nil,
    })

    if not res.absorbed then
        return false
    end

    if res.usedUtsusemi then
        target:setMod(xi.mod.UTSUSEMI, res.remaining)
        if res.setIcon then
            local effect = target:getStatusEffect(xi.effect.COPY_IMAGE)
            if effect then
                effect:setIcon(res.icon)
            end
        end
    elseif res.usedBlink then
        target:setMod(xi.mod.BLINK, res.remaining)
    end

    if res.delCopyImage then
        target:delStatusEffect(xi.effect.COPY_IMAGE)
    end

    if res.delBlink then
        target:delStatusEffect(xi.effect.BLINK)
    end

    return true
end

-----------------------------------
-- Pure: multi-attack bonus hits (slice 6753 / internal/wsmulti.BonusHits)
-- Exclusive QA→TA→DA→mythic OA thrice→OA twice ladder; Jump double stack;
-- Jump PC weapon-hit OaX fallback. Rolls inject d100 (1..100); host preserves
-- short-circuit RNG order when assembling injects.
-- params: doubleRate, tripleRate, quadRate, oaThriceRate, oaTwiceRate,
--         jumpDouble, firstHit, isJump, isPC, offHand,
--         quadRoll, tripleRoll, doubleRoll, oaThriceRoll, oaTwiceRoll,
--         weaponHitCount
-----------------------------------
xi.weaponskills.quadBonusHits   = 3
xi.weaponskills.tripleBonusHits = 2
xi.weaponskills.doubleBonusHits = 1

local function multiAttackRollProc(roll, rate)
    return roll >= 1 and roll <= rate
end

xi.weaponskills.multiAttackBonusHitsFromParams = function(params)
    params = params or {}
    local doubleRate = params.doubleRate or 0
    if params.isJump then
        doubleRate = doubleRate + (params.jumpDouble or 0)
    end

    local bonus = 0
    if multiAttackRollProc(params.quadRoll or 0, params.quadRate or 0) then
        bonus = xi.weaponskills.quadBonusHits
    elseif multiAttackRollProc(params.tripleRoll or 0, params.tripleRate or 0) then
        bonus = xi.weaponskills.tripleBonusHits
    elseif multiAttackRollProc(params.doubleRoll or 0, doubleRate) then
        bonus = xi.weaponskills.doubleBonusHits
    elseif params.firstHit and multiAttackRollProc(params.oaThriceRoll or 0, params.oaThriceRate or 0) then
        bonus = xi.weaponskills.tripleBonusHits -- thrice = +2
    elseif params.firstHit and multiAttackRollProc(params.oaTwiceRoll or 0, params.oaTwiceRate or 0) then
        bonus = xi.weaponskills.doubleBonusHits -- twice = +1
    end

    -- Jump OaX: when multi-attack rates miss, PC weapon multihit still contributes.
    if params.isJump and bonus == 0 and params.isPC then
        bonus = (params.weaponHitCount or 0) - 1
    end

    return bonus
end

-- Entity host: inject rates/RNG (short-circuit order) → pure → del charge effects.
local function getMultiAttacks(attacker, target, wsParams, firstHit, offHand)
    local doubleRate   = attacker:getMod(xi.mod.DOUBLE_ATTACK) + attacker:getMerit(xi.merit.DOUBLE_ATTACK_RATE)
    local tripleRate   = attacker:getMod(xi.mod.TRIPLE_ATTACK) + attacker:getMerit(xi.merit.TRIPLE_ATTACK_RATE)
    local quadRate     = attacker:getMod(xi.mod.QUAD_ATTACK)
    local oaThriceRate = attacker:getMod(xi.mod.MYTHIC_OCC_ATT_THRICE)
    local oaTwiceRate  = attacker:getMod(xi.mod.MYTHIC_OCC_ATT_TWICE)
    local isJump       = wsParams.isJump or false
    local jumpDouble   = isJump and attacker:getMod(xi.mod.JUMP_DOUBLE_ATTACK) or 0

    -- TODO: Assasin vest +2 Ambush augment.
    -- The logic here wasnt actually checking for the augment.
    -- Also, it was in a completely different scale, making triple attack trigger always.

    -- Preserve short-circuit RNG stream: only draw later rolls when earlier miss.
    local inject = {
        doubleRate   = doubleRate,
        tripleRate   = tripleRate,
        quadRate     = quadRate,
        oaThriceRate = oaThriceRate,
        oaTwiceRate  = oaTwiceRate,
        jumpDouble   = jumpDouble,
        firstHit     = firstHit,
        isJump       = isJump,
        isPC         = attacker:isPC(),
        offHand      = offHand,
        quadRoll     = 0,
        tripleRoll   = 0,
        doubleRoll   = 0,
        oaThriceRoll = 0,
        oaTwiceRoll  = 0,
        weaponHitCount = 0,
    }

    inject.quadRoll = math.random(1, 100)
    if inject.quadRoll > quadRate then
        inject.tripleRoll = math.random(1, 100)
        if inject.tripleRoll > tripleRate then
            inject.doubleRoll = math.random(1, 100)
            local effectiveDouble = doubleRate + jumpDouble
            if inject.doubleRoll > effectiveDouble then
                if firstHit then
                    inject.oaThriceRoll = math.random(1, 100)
                    if inject.oaThriceRoll > oaThriceRate then
                        inject.oaTwiceRoll = math.random(1, 100)
                    end
                end
            end
        end
    end

    if isJump and attacker:isPC() then
        inject.weaponHitCount = attacker:getWeaponHitCount(offHand)
    end

    local bonusHits = xi.weaponskills.multiAttackBonusHitsFromParams(inject)

    attacker:delStatusEffect(xi.effect.ASSASSINS_CHARGE)
    attacker:delStatusEffect(xi.effect.WARRIORS_CHARGE)

    return bonusHits
end

---@param attacker CBaseEntity
---@param target CBaseEntity
---@param bonus number
---@return number
xi.weaponskills.getRangedHitRate = function(attacker, target, bonus)
    return xi.combat.physicalHitRate.getRangedHitRate(attacker, target, bonus, true)
end

-- Function to calculate if a hit in a WS misses, criticals, and the respective damage done
-- Pure residual getSingleHitDamage bookkeeping (OmegaXI slice 6671).

-- ignoredDefense table present (not its values).
xi.weaponskills.ignoresDefense = function(hasIgnoredDefenseTable)
    return hasIgnoredDefenseTable
end

xi.weaponskills.attackTypeIsPhysical = function(attackType)
    return attackType == xi.attackType.PHYSICAL
end

-- Pre-block product for WS hits: (dmg + consumeMana) * ftp * pdif (no floor).
xi.weaponskills.singleHitBlockReductionInput = function(dmg, consumeMana, ftp, pdif)
    return (dmg + consumeMana) * ftp * pdif
end

-- Guard branch: physical and isGuarded → drop pDIF by 1 (floored at 0) and count.
xi.weaponskills.planSingleHitGuard = function(isPhysical, isGuarded, pdif)
    if not isPhysical or not isGuarded then
        return { applyGuard = false, newPDIF = pdif, guardedHitsDelta = 0 }
    end

    return {
        applyGuard       = true,
        newPDIF          = xi.weaponskills.guardedPDIF(pdif),
        guardedHitsDelta = 1,
    }
end

-- Successful hit bookkeeping after evade/parry/shadow: hitsLanded++, optional guard, crit sticky.
xi.weaponskills.planSingleHitSuccess = function(hitsLanded, guardedHits, pdif, criticalHit, wasCritical, isPhysical, isGuarded)
    local guard = xi.weaponskills.planSingleHitGuard(isPhysical, isGuarded, pdif)

    return {
        hitsLanded  = hitsLanded + 1,
        guardedHits = guardedHits + guard.guardedHitsDelta,
        pdif        = guard.newPDIF,
        criticalHit = criticalHit or wasCritical,
    }
end

-- Offhand multi-attack landed plan (always offhandHitsLanded++; no classify).
xi.weaponskills.planOffhandMultiLanded = function(hitDmg, isJump)
    if hitDmg <= 0 then
        return { applies = false, trySkillUp = false, addJumpTP = false }
    end

    return { applies = true, trySkillUp = true, addJumpTP = isJump }
end

-- Pure evade/miss gate for one weaponskill hit once the miss roll is known.
-- mustMiss always misses; otherwise miss when roll exceeds hitRate unless
-- guaranteedHit.
xi.weaponskills.singleHitMisses = function(missChance, hitRate, guaranteedHit, mustMiss)
    return (missChance > hitRate and not guaranteedHit) or mustMiss
end

-- Pure parry-eligibility gate: physical, not guaranteed, and isParried inject.
xi.weaponskills.singleHitMayParry = function(isPhysical, guaranteedHit)
    return isPhysical and not guaranteedHit
end

-- Pure shadow-absorb eligibility: not guaranteed and not ignoreShadows.
xi.weaponskills.singleHitMayShadowAbsorb = function(guaranteedHit, ignoreShadows)
    return not guaranteedHit and not ignoreShadows
end

-- Pure critical OR of the three weaponskill sources.
xi.weaponskills.singleHitIsCritical = function(critVaries, critChance, critRate, forcedFirstCrit, mightyStrikesApplicable)
    return (critVaries and critChance <= critRate) or forcedFirstCrit or mightyStrikesApplicable
end

-- Pure hit damage after pDIF inject, before modifyMeleeHitDamage:
--   (dmg + consumeMana) * ftp * pdif
--   if blocked: subtract block reduction (no intermediate floor)
xi.weaponskills.singleHitDamage = function(dmg, consumeMana, ftp, pdif, blocked, blockReduction)
    local hitDamage = (dmg + consumeMana) * ftp * pdif

    if blocked then
        hitDamage = hitDamage - blockReduction
    end

    return hitDamage
end

-- Pure pDIF drop applied when a physical hit is guarded: max(pdif - 1, 0).
xi.weaponskills.guardedPDIF = function(pdif)
    return math.max(pdif - 1.0, 0)
end

-----------------------------------
-- Pure: getSingleHitDamage outcome product (slice 6766)
-- Parity: internal/wsformula SingleHitOutcome
-----------------------------------
-- params: missed, parried, shadowAbsorbed, critVaries, critChance, critRate,
--   forcedFirstCrit, mightyStrikes, dmg, consumeMana, ftp, pdif, blocked,
--   blockReduction, hitsLanded, guardedHits, criticalHit, isPhysical, isGuarded
-- returns: hitDamage, hitsLanded, guardedHits, pdif, criticalHit, shadowsDelta
xi.weaponskills.singleHitOutcomeFromParams = function(params)
    params = params or {}
    local hitsLanded  = params.hitsLanded or 0
    local guardedHits = params.guardedHits or 0
    local pdif        = params.pdif or 0
    local criticalHit = params.criticalHit or false

    if params.missed or params.parried then
        return 0, hitsLanded, guardedHits, pdif, criticalHit, 0
    end

    if params.shadowAbsorbed then
        return 0, hitsLanded, guardedHits, pdif, criticalHit, 1
    end

    local wasCritical = xi.weaponskills.singleHitIsCritical(
        not not params.critVaries,
        params.critChance or 0,
        params.critRate or 0,
        params.forcedFirstCrit,
        params.mightyStrikes
    )

    local hitDamage = xi.weaponskills.singleHitDamage(
        params.dmg or 0,
        params.consumeMana or 0,
        params.ftp or 0,
        pdif,
        params.blocked,
        params.blockReduction or 0
    )

    local success = xi.weaponskills.planSingleHitSuccess(
        hitsLanded,
        guardedHits,
        pdif,
        criticalHit,
        wasCritical,
        params.isPhysical,
        params.isGuarded
    )

    return hitDamage, success.hitsLanded, success.guardedHits, success.pdif, success.criticalHit, 0
end

-- Host residual: miss/parry/shadow rolls, PDIF entity assembly, block/guard hosts.
-- Pure product: singleHitOutcomeFromParams (slice 6766).
local function getSingleHitDamage(attacker, target, dmg, ftp, wsParams, calcParams)
    local atkMultiplier        = xi.weaponskills.fTP(calcParams.tpUsed, wsParams.atkVaries)
    local ignoreDefMultiplier  = xi.weaponskills.fTP(calcParams.tpUsed, wsParams.ignoredDefense)
    local applyLevelCorrection = xi.data.levelCorrection.isLevelCorrectedZone(attacker)
    local ignoresDefense       = xi.weaponskills.ignoresDefense(wsParams.ignoredDefense ~= nil)
    local isPhysical           = xi.weaponskills.attackTypeIsPhysical(calcParams.attackType)

    -- priority order of checks
    -- evade > parry > shadow/blink > guard/block

    local missChance = math.random()
    local missed = xi.weaponskills.singleHitMisses(
        missChance, calcParams.hitRate, calcParams.guaranteedHit, calcParams.mustMiss)

    local parried = false
    if
        not missed and
        xi.weaponskills.singleHitMayParry(isPhysical, calcParams.guaranteedHit) and
        xi.combat.physical.isParried(target, attacker)
    then
        parried = true
    end

    local shadowAbsorbed = false
    if
        not missed and
        not parried and
        xi.weaponskills.singleHitMayShadowAbsorb(calcParams.guaranteedHit, wsParams.ignoreShadows) and
        shadowAbsorb(target)
    then
        shadowAbsorbed = true
    end

    local critChance = 0
    local wasCritical = false
    local blocked = false
    local blockReduction = 0
    local consumeMana = 0
    local isGuarded = false

    if not missed and not parried and not shadowAbsorbed then
        critChance = math.random()
        wasCritical = xi.weaponskills.singleHitIsCritical(
            not not wsParams.critVaries,
            critChance,
            calcParams.critRate,
            calcParams.forcedFirstCrit,
            calcParams.mightyStrikesApplicable
        )

        if calcParams.attackType == xi.attackType.PHYSICAL then
            calcParams.pdif = xi.combat.physical.calculateMeleePDIF(
                attacker, target, calcParams.attackInfo.weaponType, atkMultiplier,
                wasCritical, applyLevelCorrection, ignoresDefense, ignoreDefMultiplier,
                true, calcParams.attackInfo.slot, false)
        else
            calcParams.pdif = xi.combat.physical.calculateRangedPDIF(
                attacker, target, calcParams.skillType, atkMultiplier, wasCritical,
                applyLevelCorrection, ignoresDefense, ignoreDefMultiplier, true, 0)
        end

        consumeMana = xi.combat.damage.consumeManaAddition(attacker)
        blocked = xi.combat.physical.isBlocked(target, attacker)
        if blocked then
            -- Weaponskill path does not floor before the block-reduction host.
            blockReduction = xi.combat.physical.getDamageReductionForBlock(
                target,
                attacker,
                xi.weaponskills.singleHitBlockReductionInput(dmg, consumeMana, ftp, calcParams.pdif)
            )
        end

        isGuarded = xi.combat.physical.isGuarded(target, attacker)
    end

    local hitDamage, hitsLanded, guardedHits, pdif, criticalHit, shadowsDelta =
        xi.weaponskills.singleHitOutcomeFromParams({
            missed          = missed,
            parried         = parried,
            shadowAbsorbed  = shadowAbsorbed,
            critVaries      = not not wsParams.critVaries,
            critChance      = critChance,
            critRate        = calcParams.critRate,
            forcedFirstCrit = calcParams.forcedFirstCrit,
            mightyStrikes   = calcParams.mightyStrikesApplicable,
            dmg             = dmg,
            consumeMana     = consumeMana,
            ftp             = ftp,
            pdif            = calcParams.pdif or 0,
            blocked         = blocked,
            blockReduction  = blockReduction,
            hitsLanded      = calcParams.hitsLanded,
            guardedHits     = calcParams.guardedHits,
            criticalHit     = calcParams.criticalHit,
            isPhysical      = isPhysical,
            isGuarded       = isGuarded,
        })

    calcParams.hitsLanded      = hitsLanded
    calcParams.guardedHits     = guardedHits
    calcParams.pdif            = pdif
    calcParams.criticalHit     = criticalHit
    calcParams.shadowsAbsorbed = (calcParams.shadowsAbsorbed or 0) + shadowsDelta

    return hitDamage, calcParams
end

-- Maps a weaponskill weapon skill type onto the physical SDT mod family used
-- by modifyMeleeHitDamage. Dagger and Polearm share PIERCE; Club and Staff
-- share IMPACT; everything else (including swords/axes/etc.) is SLASH.
-- Returns the xi.mod id so callers can getMod without re-branching.
xi.weaponskills.meleeHitSDTMod = function(weaponType)
    if weaponType == xi.skill.HAND_TO_HAND then
        return xi.mod.HTH_SDT
    elseif
        weaponType == xi.skill.DAGGER or
        weaponType == xi.skill.POLEARM
    then
        return xi.mod.PIERCE_SDT
    elseif
        weaponType == xi.skill.CLUB or
        weaponType == xi.skill.STAFF
    then
        return xi.mod.IMPACT_SDT
    end

    return xi.mod.SLASH_SDT
end

-- Unclamped SDT scale for weaponskill melee hits: 1 + sdtMod/10000.
-- Distinct from xi.combat.damage.physicalElementSDT, which clamps to [0, 3].
xi.weaponskills.meleeHitSDTScale = function(sdtMod)
    return 1 + sdtMod / 10000
end

-- Pure product half of modifyMeleeHitDamage once physicalDmgTaken, SDT scale,
-- Scarlet Delirium, and Souleater are injected.
--
--	if formless: dmg = raw
--	else:        dmg = physicalDmgTakenResult * sdtScale
--	dmg = dmg * scarletMult + souleaterAdd
--
-- Phalanx and Stoneskin remain host residual after this product.
xi.weaponskills.modifyMeleeHitDamageProduct = function(rawDamage, formless, physicalDmgTakenResult, sdtScale, scarletMult, souleaterAdd)
    local adjustedDamage = rawDamage

    if not formless then
        adjustedDamage = physicalDmgTakenResult * sdtScale
    end

    adjustedDamage = adjustedDamage * scarletMult
    adjustedDamage = adjustedDamage + souleaterAdd

    return adjustedDamage
end

local function modifyMeleeHitDamage(attacker, target, attackTbl, wsParams, rawDamage)
    local physicalTaken = rawDamage
    local sdtScale      = 1

    if not wsParams.formless then
        physicalTaken = target:physicalDmgTaken(rawDamage, attackTbl.damageType)
        sdtScale      = xi.weaponskills.meleeHitSDTScale(target:getMod(xi.weaponskills.meleeHitSDTMod(attackTbl.weaponType)))
    end

    local adjustedDamage = xi.weaponskills.modifyMeleeHitDamageProduct(
        rawDamage,
        wsParams.formless,
        physicalTaken,
        sdtScale,
        xi.combat.damage.scarletDeliriumMultiplier(attacker),
        xi.combat.damage.souleaterAddition(attacker)
    )

    adjustedDamage = utils.handlePhalanx(target, adjustedDamage)
    adjustedDamage = utils.handleStoneskin(target, adjustedDamage)

    return adjustedDamage
end

-- Compute magic damage component of hybrid weaponskill
-- https://wiki.ffo.jp/html/1261.html
-- https://www.ffxiah.com/forum/topic/33470/the-sealed-dagger-a-ninja-guide/151/#3420836
-- https://www.ffxiah.com/forum/topic/49614/blade-chi-damage-formula/2/#3171538
--
-- Pure base product once fTP, MAGIC_DAMAGE, ALL_WSDMG_ALL_HITS, and optional
-- per-WS WSD are resolved. Host residual continues with addBonusesAbility,
-- then hybridWeaponskillMagicBonusFTP, then resist / shell / severe /
-- absorb / Phalanx / OneForAll / Stoneskin.
--
-- Distinct from xi.mobskills.hybridMagicDamage (fixed 0.5 hybrid scale, no WSD).
xi.weaponskills.hybridWeaponskillMagicBase = function(physicaldmg, ftp, magicDamageMod, allWSDMG, perWSWSD)
    local magicdmg = math.floor(physicaldmg * ftp + magicDamageMod)
    local wsd      = allWSDMG

    -- Per-weaponskill WSD only stacks when the mod is strictly positive.
    if perWSWSD > 0 then
        wsd = wsd + perWSWSD
    end

    return math.floor(magicdmg * (100 + wsd) / 100)
end

-- bonusfTP add after ability bonuses (upstream order: base → ability bonuses →
-- bonusfTP → resist…). Kept pure so the floor product is pin-able.
xi.weaponskills.hybridWeaponskillMagicBonusFTP = function(magicdmg, physicaldmg, bonusFTP)
    return math.floor(magicdmg + bonusFTP * physicaldmg)
end

-----------------------------------
-- Pure: hybrid WS magic product composition (slice 6769 / 6652 / 6676)
-- Parity: internal/wsformula HybridWeaponskillMagic*
-----------------------------------

-- Mid-product after ability bonuses, through bonus-fTP and resist/shell floors.
-- Host applies handleSevereDamage to the result.
-- params: afterAbility, physicaldmg, hasBonusFTP, bonusFTP, resist, damageAdj
xi.weaponskills.hybridWeaponskillMagicPreSevereFromParams = function(params)
    params = params or {}
    local bonusFTP = xi.weaponskills.bonusFTPOrZero(
        not not params.hasBonusFTP, params.bonusFTP or 0)
    local dmg = xi.weaponskills.hybridWeaponskillMagicBonusFTP(
        params.afterAbility or 0,
        params.physicaldmg or 0,
        bonusFTP
    )
    return xi.weaponskills.magicMitigationFloors(
        dmg, params.resist or 1, params.damageAdj or 1)
end

-- Post-severe product through final floor. When useAfterMitigate and damage
-- still warrants Phalanx/OFA/Stoneskin, afterMitigate is the host result.
-- params: afterSevere, absorb, nullify, afterMitigate, useAfterMitigate
xi.weaponskills.hybridWeaponskillMagicFinalFromParams = function(params)
    params = params or {}
    local dmg = xi.weaponskills.hybridMagicPostSevere(
        params.afterSevere or 0,
        params.absorb or 1,
        params.nullify or 1
    )
    if xi.weaponskills.hybridMagicShouldMitigate(dmg) and params.useAfterMitigate then
        dmg = params.afterMitigate or 0
    end
    return xi.weaponskills.floorHybridMagicFinal(dmg)
end

-- Full product once entity residual injects are known.
-- params: physicaldmg, ftp, magicDamageMod, allWSDMG, perWSWSD,
--   afterAbility + useAfterAbility, hasBonusFTP, bonusFTP, resist, damageAdj,
--   afterSevere + useAfterSevere, absorb, nullify,
--   afterMitigate + useAfterMitigate
xi.weaponskills.hybridWeaponskillMagicFromParams = function(params)
    params = params or {}
    local dmg = xi.weaponskills.hybridWeaponskillMagicBase(
        params.physicaldmg or 0,
        params.ftp or 0,
        params.magicDamageMod or 0,
        params.allWSDMG or 0,
        params.perWSWSD or 0
    )
    if params.useAfterAbility then
        dmg = params.afterAbility or 0
    end

    local pre = xi.weaponskills.hybridWeaponskillMagicPreSevereFromParams({
        afterAbility = dmg,
        physicaldmg  = params.physicaldmg or 0,
        hasBonusFTP  = params.hasBonusFTP,
        bonusFTP     = params.bonusFTP or 0,
        resist       = params.resist or 1,
        damageAdj    = params.damageAdj or 1,
    })

    local afterSevere = pre
    if params.useAfterSevere then
        afterSevere = params.afterSevere or 0
    end

    return xi.weaponskills.hybridWeaponskillMagicFinalFromParams({
        afterSevere      = afterSevere,
        absorb           = params.absorb or 1,
        nullify          = params.nullify or 1,
        afterMitigate    = params.afterMitigate or 0,
        useAfterMitigate = params.useAfterMitigate,
    })
end

-- Host residual: addBonusesAbility, resist rate, damage adj, severe damage,
-- absorb/nullify, Phalanx/OFA/Stoneskin. Pure product: hybridWeaponskillMagic*.
local function calculateHybridMagicDamage(tp, physicaldmg, attacker, target, wsParams, calcParams, wsID)
    local ftp = xi.weaponskills.fTP(tp, wsParams.ftpMod)
    local base = xi.weaponskills.hybridWeaponskillMagicBase(
        physicaldmg,
        ftp,
        attacker:getMod(xi.mod.MAGIC_DAMAGE),
        attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
        attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID)
    )

    local afterAbility = math.floor(addBonusesAbility(attacker, wsParams.ele, target, base, wsParams))
    local resist       = xi.combat.magicHitRate.calculateResistRate(
        attacker, target, 0, wsParams.skill, 0, wsParams.ele, 0, 0, calcParams.bonusAcc)
    local damageAdj    = xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false)

    local preSevere = xi.weaponskills.hybridWeaponskillMagicPreSevereFromParams({
        afterAbility = afterAbility,
        physicaldmg  = physicaldmg,
        hasBonusFTP  = calcParams.bonusfTP ~= nil,
        bonusFTP     = calcParams.bonusfTP or 0,
        resist       = resist,
        damageAdj    = damageAdj,
    })

    local afterSevere = math.floor(target:handleSevereDamage(preSevere, false))

    local absorb  = 1
    local nullify = 1
    if xi.weaponskills.hybridMagicPositive(afterSevere) then
        absorb  = xi.spells.damage.calculateAbsorption(target, wsParams.ele, true)
        nullify = xi.spells.damage.calculateNullification(target, wsParams.ele, true, false)
    end

    local afterAbsorb = xi.weaponskills.hybridMagicPostSevere(afterSevere, absorb, nullify)
    local afterMitigate = afterAbsorb
    local useAfterMitigate = false
    if xi.weaponskills.hybridMagicShouldMitigate(afterAbsorb) then
        afterMitigate = utils.handlePhalanx(target, afterAbsorb)
        afterMitigate = utils.handleOneForAll(target, afterMitigate)
        afterMitigate = utils.handleStoneskin(target, afterMitigate)
        useAfterMitigate = true
    end

    return xi.weaponskills.hybridWeaponskillMagicFromParams({
        physicaldmg      = physicaldmg,
        ftp              = ftp,
        magicDamageMod   = attacker:getMod(xi.mod.MAGIC_DAMAGE),
        allWSDMG         = attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
        perWSWSD         = attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID),
        useAfterAbility  = true,
        afterAbility     = afterAbility,
        hasBonusFTP      = calcParams.bonusfTP ~= nil,
        bonusFTP         = calcParams.bonusfTP or 0,
        resist           = resist,
        damageAdj        = damageAdj,
        useAfterSevere   = true,
        afterSevere      = afterSevere,
        absorb           = absorb,
        nullify          = nullify,
        useAfterMitigate = useAfterMitigate,
        afterMitigate    = afterMitigate,
    })
end

-- returns ammo used, if any
local function useAmmo(attacker)
    return xi.weaponskills.useAmmoCount(xi.combat.ranged.shouldUseAmmo(attacker))
end

-- Calculates the raw damage for a weaponskill, used by both xi.weaponskills.doPhysicalWeaponskill and xi.weaponskills.doRangedWeaponskill.
-- Behavior of damage calculations can vary based on the passed in calcParams, which are determined by the calling function
-- depending on the type of weaponskill being done, and any special cases for that weaponskill
--
-- wsParams can contain: ftpMod, str_wsc, dex_wsc, vit_wsc, int_wsc, mnd_wsc, critVaries,
-- accVaries, ignoredDefense, atkVaries, kick, hybridWS, hitsHigh, formless
--
-- See xi.weaponskills.doPhysicalWeaponskill or xi.weaponskills.doRangedWeaponskill for how calcParams are determined.

-- Pure fTP for a weaponskill hit: hybrid first hits use 1 + bonusfTP only;
-- otherwise fTP(tp, ftpMod) + bonusfTP.
xi.weaponskills.weaponskillFTP = function(tp, ftpMod, bonusFTP, hybridHit)
    if hybridHit then
        return 1 + bonusFTP
    end

    return xi.weaponskills.fTP(tp, ftpMod) + bonusFTP
end

-- Pure Sneak Attack first-hit add (THF main path): floor(final + pdif * floor(DEX * (1 + SNEAK_ATK_DEX/100))).
xi.weaponskills.weaponskillSneakAttackBonus = function(finalDmg, pdif, dex, sneakAtkDexMod)
    local dexFactor = math.floor(dex * (1 + sneakAtkDexMod / 100))

    return math.floor(finalDmg + pdif * dexFactor)
end

-----------------------------------
-- Pure: first-hit SA/TA/augment product (slice 6771 / 6662)
-- Parity: internal/wsformula FirstHitSATAProduct
-----------------------------------
-- params: finalDmg, pdif, isTHFMain, sneakApplicable, trickApplicable,
--   dex, sneakAtkDexMod, agi, trickAtkAgiMod, augmentsSA, augmentsTA
xi.weaponskills.firstHitSATAProductFromParams = function(params)
    params = params or {}
    local dmg = params.finalDmg or 0

    if params.isTHFMain and params.sneakApplicable then
        dmg = xi.weaponskills.weaponskillSneakAttackBonus(
            dmg, params.pdif or 0, params.dex or 0, params.sneakAtkDexMod or 0)
    end

    if params.isTHFMain and params.trickApplicable then
        dmg = xi.weaponskills.weaponskillTrickAttackBonus(
            dmg, params.pdif or 0, params.agi or 0, params.trickAtkAgiMod or 0)
    end

    if params.sneakApplicable then
        dmg = xi.weaponskills.weaponskillAugmentSA(dmg, params.augmentsSA or 0)
    end

    if params.trickApplicable then
        dmg = xi.weaponskills.weaponskillAugmentTA(dmg, params.augmentsTA or 0)
    end

    return dmg
end

-- Pure Trick Attack first-hit add (THF main path): floor(final + pdif * floor(AGI * (1 + TRICK_ATK_AGI/100))).
xi.weaponskills.weaponskillTrickAttackBonus = function(finalDmg, pdif, agi, trickAtkAgiMod)
    local agiFactor = math.floor(agi * (1 + trickAtkAgiMod / 100))

    return math.floor(finalDmg + pdif * agiFactor)
end

-- Pure AUGMENTS_SA multiply (any job with SA applicable).
xi.weaponskills.weaponskillAugmentSA = function(finalDmg, augmentsSA)
    return math.floor(finalDmg * (1 + augmentsSA / 100))
end

-- Pure AUGMENTS_TA multiply (any job with TA applicable).
xi.weaponskills.weaponskillAugmentTA = function(finalDmg, augmentsTA)
    return math.floor(finalDmg * (1 + augmentsTA / 100))
end

-- Pure first-hit-only WSD bonus store: finalDmg * ALL_WSDMG_FIRST_HIT / 100.
xi.weaponskills.weaponskillFirstHitBonus = function(finalDmg, firstHitWSDMod)
    return finalDmg * firstHitWSDMod / 100
end

-- Pure multi-hit fTP carry: keep ftp when multiHitfTP, else reset to 1 for later hits.
xi.weaponskills.multiHitFTPAfterFirst = function(ftp, multiHitfTP)
    if multiHitfTP then
        return ftp
    end

    return 1
end

-- Remaining mainhand hits after the first is accounted for (numHits includes first).
xi.weaponskills.mainhandHitsRemaining = function(numHits)
    return numHits - 1
end

-- Pure TP-counter classification for a landed non-first multi-hit swing.
-- Returns 'tp', 'main', or 'offhand' for which counter to increment.
--
--	if (isH2H or extraOffhandHit) and hitsDone == 1: 'tp'   -- second swing counts as TP hit
--	elseif isBarrage: 'tp'
--	elseif isOffhand: 'offhand'
--	else: 'main'
xi.weaponskills.classifyLandedHitTP = function(isH2H, extraOffhandHit, hitsDone, isBarrage, isOffhand)
    if
        (isH2H or extraOffhandHit) and
        hitsDone == 1
    then
        return 'tp'
    elseif isBarrage then
        return 'tp'
    elseif isOffhand then
        return 'offhand'
    end

    return 'main'
end

-- Pure offhand hit base (no floor): weaponDamage[2] + fSTR + wsc * alpha.
xi.weaponskills.offhandBaseDamage = function(offhandWeaponDamage, fSTR, wsc, alpha)
    return offhandWeaponDamage + fSTR + wsc * alpha
end

-- Pure multi-proc budget: at most two multi procs are checked on the mainhand
-- numHits loop. Returns the updated numMultiProcs after an extraMultis roll.
xi.weaponskills.nextMultiProcCount = function(numMultiProcs, extraMultis)
    if extraMultis > 0 then
        return numMultiProcs + 1
    end

    return numMultiProcs
end

-- Pure all-hits WSD product for non-Jump weaponskills:
--   bonus = ALL_WSDMG_ALL_HITS + (perWS if perWS > 0 and not pet)
--   final = final * (100 + bonus) / 100 + firstHitBonus
-- Jump skips entirely (returns finalDmg unchanged).
xi.weaponskills.allHitsWSDProduct = function(finalDmg, allWSDMG, perWSWSD, isPet, firstHitBonus, isJump)
    if isJump then
        return finalDmg
    end

    local bonusdmg = allWSDMG

    if perWSWSD > 0 and not isPet then
        bonusdmg = bonusdmg + perWSWSD
    end

    finalDmg = finalDmg * (100 + bonusdmg) / 100
    finalDmg = finalDmg + firstHitBonus

    return finalDmg
end

-- Pure hit-loop stop: retail caps at 8 swings or when cumulative damage reaches target HP+SS.
xi.weaponskills.wsHitLoopContinues = function(hitsDone, finalDmg, targetHp)
    return hitsDone < 8 and finalDmg < targetHp
end

-- Pure ammo exhaustion: when tracking ammo (ammoCount ~= -1) and used >= count,
-- force the hit loop to stop (hitsDone = 8).
xi.weaponskills.ammoExhausted = function(ammoUsed, ammoCount)
    return ammoCount ~= -1 and ammoUsed >= ammoCount
end

-- Pure first-hit accuracy bonus used for physical WS firstHitRate (+100).
xi.weaponskills.firstHitAccuracyBonus = function(bonusAcc)
    return bonusAcc + 100
end

-- Pure whether to temporarily swap hitRate to firstHitRate for the first swing.
xi.weaponskills.shouldUseFirstHitRate = function(isJump, hasFirstHitRate)
    return not isJump and hasFirstHitRate
end

-- Pure target HP used for the multi-hit stop gate (current HP + Stoneskin mod).
xi.weaponskills.weaponskillTargetHp = function(targetHp, stoneskinMod)
    return targetHp + stoneskinMod
end

-- Pure mainhand multi-attack count: ranged never rolls multi-attacks on the
-- mainhand multi budget (always 0).
xi.weaponskills.initialMainhandMultis = function(isRanged, multiBonusHits)
    if isRanged then
        return 0
    end

    return multiBonusHits
end

-- Pure initial multi-proc counter: one proc if any mainhand multis, else zero.
xi.weaponskills.initialMultiProcCount = function(numMainHandMultis)
    if numMainHandMultis > 0 then
        return 1
    end

    return 0
end

-- Pure useAmmo result: 1 if shouldUseAmmo, else 0.
xi.weaponskills.useAmmoCount = function(shouldUseAmmo)
    if shouldUseAmmo then
        return 1
    end

    return 0
end

-- Pure magic-WS ranged slot gate: archery or marksmanship.
xi.weaponskills.isRangedMagicWeaponskill = function(skill)
    return skill == xi.skill.MARKSMANSHIP or skill == xi.skill.ARCHERY
end

-- Pure hybrid/magic resist+adjustment floors after ability bonuses:
--   floor(floor(dmg * resist) * damageAdj)
-- Host then applies handleSevereDamage to the result.
xi.weaponskills.magicMitigationFloors = function(dmg, resist, damageAdj)
    return math.floor(math.floor(dmg * resist) * damageAdj)
end

-- Pure hybrid absorb/nullify when damage is still positive after severe:
-- each multiply is floored (distinct from magic WS which does not floor here).
xi.weaponskills.hybridMagicAbsorbNullify = function(magicdmg, absorb, nullify)
    if magicdmg <= 0 then
        return magicdmg
    end

    magicdmg = math.floor(magicdmg * absorb)
    magicdmg = math.floor(magicdmg * nullify)

    return magicdmg
end

-- Pure residual calculateHybridMagicDamage gates (OmegaXI slice 6676).

-- calcParams.bonusfTP or 0
xi.weaponskills.bonusFTPOrZero = function(hasBonusFTP, bonusFTP)
    if not hasBonusFTP then
        return 0
    end

    return bonusFTP
end

xi.weaponskills.hybridMagicPositive = function(magicdmg)
    return magicdmg > 0
end

-- Absorb/nullify step after severe: only when still positive.
xi.weaponskills.hybridMagicPostSevere = function(magicdmg, absorb, nullify)
    if not xi.weaponskills.hybridMagicPositive(magicdmg) then
        return magicdmg
    end

    return xi.weaponskills.hybridMagicAbsorbNullify(magicdmg, absorb, nullify)
end

-- Second positive gate for Phalanx / OneForAll / Stoneskin after absorb.
xi.weaponskills.hybridMagicShouldMitigate = function(magicdmg)
    return xi.weaponskills.hybridMagicPositive(magicdmg)
end

xi.weaponskills.floorHybridMagicFinal = function(magicdmg)
    return math.floor(magicdmg)
end

-- Pure magic-WS absorb/nullify when damage is non-negative (no intermediate floors).
xi.weaponskills.magicWeaponskillAbsorbNullify = function(dmg, absorb, nullify)
    return dmg * absorb * nullify
end

-- Pure residual calculateRawWSDmg bookkeeping (OmegaXI slice 6670).

-- Ammo inventory init: non-PC/non-ranged → -1 (no tracking); missing item → 0.
xi.weaponskills.rangedAmmoInventoryCount = function(isRanged, isPC, hasAmmoItem, quantity)
    if not isRanged or not isPC then
        return -1
    end

    if not hasAmmoItem then
        return 0
    end

    return quantity
end

xi.weaponskills.ammoTrackingActive = function(ammoCount)
    return ammoCount ~= -1
end

-- First-hit skill-up plan. Quirk: mainHitsLanded is forced to 0, tpHitsLanded = 1.
xi.weaponskills.planFirstHitLanded = function(skillTypePresent, hitDmg, isJump)
    if not skillTypePresent or hitDmg <= 0 then
        return { applies = false, trySkillUp = false, addJumpTP = false, tpHitsLanded = 0, mainHitsLanded = 0 }
    end

    return {
        applies        = true,
        trySkillUp     = true,
        addJumpTP      = isJump,
        tpHitsLanded   = 1,
        mainHitsLanded = 0,
    }
end

-- Subsequent landed swing plan + TP classification injects.
xi.weaponskills.planSubsequentHitLanded = function(hitDmg, isJump, isH2H, extraOffhandHit, hitsDone, isBarrage, isOffhand)
    if hitDmg <= 0 then
        return { applies = false, trySkillUp = false, addJumpTP = false, tpClass = 'main' }
    end

    return {
        applies    = true,
        trySkillUp = true,
        addJumpTP  = isJump,
        tpClass    = xi.weaponskills.classifyLandedHitTP(isH2H, extraOffhandHit, hitsDone, isBarrage, isOffhand),
    }
end

xi.weaponskills.applyHitTPClass = function(tpClass, tpHits, mainHits, offhandHits)
    if tpClass == 'tp' then
        return tpHits + 1, mainHits, offhandHits
    elseif tpClass == 'offhand' then
        return tpHits, mainHits, offhandHits + 1
    end

    return tpHits, mainHits + 1, offhandHits
end

xi.weaponskills.planH2HOffhand = function(skillType, subSkill)
    if skillType == xi.skill.HAND_TO_HAND then
        return {
            offhandSkill      = xi.skill.HAND_TO_HAND,
            isH2H             = true,
            subTPUsesMainhand = true,
        }
    end

    return {
        offhandSkill      = subSkill,
        isH2H             = false,
        subTPUsesMainhand = false,
    }
end

xi.weaponskills.extraHitsLanded = function(mainHitsLanded, offhandHitsLanded)
    return mainHitsLanded + offhandHitsLanded
end

xi.weaponskills.planAmmoStep = function(tracking, ammoUsed, ammoCount, useAmmoCount)
    if not tracking then
        return { ammoUsed = ammoUsed, forceStop = false }
    end

    local used = ammoUsed + useAmmoCount

    return {
        ammoUsed  = used,
        forceStop = xi.weaponskills.ammoExhausted(used, ammoCount),
    }
end

xi.weaponskills.jumpTPGain = function(mainhandTPGain, attackerTPMult)
    return mainhandTPGain * attackerTPMult
end

-----------------------------------
-- Pure: pre-Adoulin WSC alpha + main-hand base D (slice 6756 / internal/wsformula)
-- Alpha: Adoulin → 1; else level bands >75 / >59 / >5 / else 1
-- MainBase: floor(weaponDmg + fSTR + bonusWSmods + wsc * alpha)
-----------------------------------
xi.weaponskills.alphaFromParams = function(params)
    params = params or {}
    if params.useAdoulinWeaponSkillChanges then
        return 1
    end

    local level = params.level or 0
    if level > 75 then
        return 0.85
    elseif level > 59 then
        return 0.9 - math.floor((level - 60) / 2) / 100
    elseif level > 5 then
        return 1 - math.floor(level / 6) / 100
    end

    return 1
end

xi.weaponskills.mainBaseFromParams = function(params)
    params = params or {}
    return math.floor(
        (params.weaponDamage or 0)
            + (params.fSTR or 0)
            + (params.bonusWSmods or 0)
            + (params.wsc or 0) * (params.alpha or 0)
    )
end

-- TODO: Reduce complexity
-- Disable cyclomatic complexity check for this function:
-- luacheck: ignore 561
xi.weaponskills.calculateRawWSDmg = function(attacker, target, wsID, tp, action, wsParams, calcParams)
    local targetLvl = target:getMainLvl()
    local targetHp  = xi.weaponskills.weaponskillTargetHp(target:getHP(), target:getMod(xi.mod.STONESKIN))

    -- Obtains alpha, used for working out WSC on legacy servers. Retail has no alpha anymore as of 2014 Weaponskill functions
    local alpha = xi.weaponskills.alphaFromParams({
        level                         = attacker:getMainLvl(),
        useAdoulinWeaponSkillChanges  = xi.settings.main.USE_ADOULIN_WEAPON_SKILL_CHANGES,
    })

    local wsc      = xi.combat.physical.calculateWSC(attacker, wsParams.str_wsc, wsParams.dex_wsc, wsParams.vit_wsc, wsParams.agi_wsc, wsParams.int_wsc, wsParams.mnd_wsc, wsParams.chr_wsc)
    local mainBase = xi.weaponskills.mainBaseFromParams({
        weaponDamage = calcParams.weaponDamage[1],
        fSTR         = calcParams.fSTR,
        bonusWSmods  = calcParams.bonusWSmods,
        wsc          = wsc,
        alpha        = alpha,
    })

    -- Calculate fTP multiplier
    local ftp = xi.weaponskills.weaponskillFTP(tp, wsParams.ftpMod, calcParams.bonusfTP, calcParams.hybridHit)

    -- Calculate critrates
    calcParams.critRate = 0
    if wsParams.critVaries then -- Work out critical hit ratios
        calcParams.critRate = xi.combat.physical.calculateSwingCriticalRate(attacker, target, tp, calcParams.attackInfo.slot, wsParams.critVaries)
    end

    -- Start the WS
    local hitsDone                = 1
    local hitdmg                  = 0
    local finaldmg                = 0
    local mainhandTPGain          = xi.combat.tp.getSingleWeaponTPReturn(attacker, xi.slot.MAIN) -- TODO: are these calculated wrong? ((delay1+delay2)/2 * 1 - DW%) = tp return for both hands?
    local subTPGain               = xi.combat.tp.getSingleWeaponTPReturn(attacker, xi.slot.SUB)  --
    local isJump                  = wsParams.isJump or false
    local attackerTPMult          = wsParams.attackerTPMult or 1
    local ammoCount               = -1
    local ammoUsed                = 0
    local isRanged                = calcParams.attackInfo.slot == xi.slot.RANGED
    calcParams.hitsLanded         = 0
    calcParams.mainHitsLanded     = 0
    calcParams.tpHitsLanded       = 0
    calcParams.shadowsAbsorbed    = 0
    calcParams.mainhandHitsLanded = 0
    calcParams.offhandHitsLanded  = 0
    calcParams.guardedHits        = 0

    -- Get ammo information
    local ammoItem = isRanged and attacker:isPC() and attacker:getEquippedItem(xi.slot.AMMO) or nil
    ammoCount = xi.weaponskills.rangedAmmoInventoryCount(
        isRanged,
        attacker:isPC(),
        ammoItem ~= nil,
        ammoItem and ammoItem:getQuantity() or 0
    )

    -- Calculate the damage from the first hit
    if xi.weaponskills.shouldUseFirstHitRate(isJump, calcParams.firstHitRate ~= nil) then
        calcParams.origHitRate = calcParams.hitRate
        calcParams.hitRate = calcParams.firstHitRate
    end

    local dmg = mainBase
    hitdmg, calcParams = getSingleHitDamage(attacker, target, dmg, ftp, wsParams, calcParams)

    if xi.weaponskills.shouldUseFirstHitRate(isJump, calcParams.origHitRate ~= nil) then
        calcParams.hitRate = calcParams.origHitRate
    end

    if calcParams.melee then
        hitdmg = modifyMeleeHitDamage(attacker, target, calcParams.attackInfo, wsParams, hitdmg)
    end

    local firstHitPlan = xi.weaponskills.planFirstHitLanded(
        not not calcParams.skillType,
        hitdmg,
        isJump
    )
    if firstHitPlan.applies then
        attacker:trySkillUp(calcParams.skillType, targetLvl)

        if firstHitPlan.addJumpTP then
            attacker:addTP(xi.weaponskills.jumpTPGain(mainhandTPGain, attackerTPMult))
        end

        calcParams.tpHitsLanded   = firstHitPlan.tpHitsLanded -- Store number of TP hits that have landed thus far
        calcParams.mainHitsLanded = firstHitPlan.mainHitsLanded
    end

    finaldmg = finaldmg + hitdmg

    -- Finish first/mainhand hit

    local numMainHandMultis = xi.weaponskills.initialMainhandMultis(isRanged, isRanged and 0 or getMultiAttacks(attacker, target, wsParams, true, false))
    local numOffhandMultis  = 0
    local numMultiProcs     = xi.weaponskills.initialMultiProcCount(numMainHandMultis)

    -- First-hit SA/TA/augment product (slice 6771). Host residual: job/status/mod injects.
    finaldmg = xi.weaponskills.firstHitSATAProductFromParams({
        finalDmg        = finaldmg,
        pdif            = calcParams.pdif or 0,
        isTHFMain       = attacker:getMainJob() == xi.job.THF,
        sneakApplicable = calcParams.sneakApplicable,
        trickApplicable = calcParams.trickApplicable,
        dex             = attacker:getStat(xi.mod.DEX),
        sneakAtkDexMod  = attacker:getMod(xi.mod.SNEAK_ATK_DEX),
        agi             = attacker:getStat(xi.mod.AGI),
        trickAtkAgiMod  = attacker:getMod(xi.mod.TRICK_ATK_AGI),
        augmentsSA      = attacker:getMod(xi.mod.AUGMENTS_SA),
        augmentsTA      = attacker:getMod(xi.mod.AUGMENTS_TA),
    })

    -- We've now accounted for any crit from SA/TA, so nullify them
    calcParams.forcedFirstCrit = false

    -- For items that apply bonus damage to the first hit of a weaponskill (but not later hits),
    -- store bonus damage for first hit, for use after other calculations are done
    local firstHitBonus = xi.weaponskills.weaponskillFirstHitBonus(finaldmg, attacker:getMod(xi.mod.ALL_WSDMG_FIRST_HIT))

    -- Reset fTP if it's not supposed to carry over across all hits for this WS
    -- We'll recalculate our mainhand damage after doing offhand
    ftp = xi.weaponskills.multiHitFTPAfterFirst(ftp, wsParams.multiHitfTP)

    local h2hPlan = xi.weaponskills.planH2HOffhand(
        calcParams.skillType or 0,
        attacker:getWeaponSkillType(xi.slot.SUB)
    )
    local offhandSkill = h2hPlan.offhandSkill
    local isH2H        = h2hPlan.isH2H
    if h2hPlan.subTPUsesMainhand then
        subTPGain = mainhandTPGain
    end

    calcParams.guaranteedHit = false -- Accuracy bonus from SA/TA applies only to first main and offhand hit

    dmg = mainBase

    -- First mainhand hit is already accounted for
    local mainhandHits     = xi.weaponskills.mainhandHitsRemaining(wsParams.numHits)
    local mainhandHitsDone = 0

    if xi.weaponskills.ammoTrackingActive(ammoCount) then
        local ammoStep = xi.weaponskills.planAmmoStep(
            true,
            ammoUsed,
            ammoCount,
            useAmmo(attacker)
        )
        ammoUsed = ammoStep.ammoUsed
        if ammoStep.forceStop then
            hitsDone = 8 -- Attack while loops will stop if hitsDone is 8 or higher
        end
    end

    -- Use up any remaining hits in the WS's numhits
    while mainhandHitsDone < mainhandHits and xi.weaponskills.wsHitLoopContinues(hitsDone, finaldmg, targetHp) do
        hitdmg, calcParams    = getSingleHitDamage(attacker, target, dmg, ftp, wsParams, calcParams)

        if calcParams.melee then
            hitdmg = modifyMeleeHitDamage(attacker, target, calcParams.attackInfo, wsParams, hitdmg)
        end

        local subPlan = xi.weaponskills.planSubsequentHitLanded(
            hitdmg,
            isJump,
            isH2H,
            calcParams.extraOffhandHit,
            hitsDone,
            wsParams.isBarrage,
            false
        )
        if subPlan.applies then
            attacker:trySkillUp(calcParams.skillType, targetLvl)

            -- When dual wielding, the mainhand appears to count the second hit as a TP hit unless it's a 1 hit WS where the offhand will gain TP
            -- H2H also does this on retail (much more easy to verify)
            -- Needs better verification
            calcParams.tpHitsLanded, calcParams.mainHitsLanded, calcParams.offhandHitsLanded =
                xi.weaponskills.applyHitTPClass(
                    subPlan.tpClass,
                    calcParams.tpHitsLanded,
                    calcParams.mainHitsLanded,
                    calcParams.offhandHitsLanded
                )

            if subPlan.addJumpTP then
                attacker:addTP(xi.weaponskills.jumpTPGain(mainhandTPGain, attackerTPMult))
            end
        end

        finaldmg                  = finaldmg + hitdmg
        hitsDone                  = hitsDone + 1
        mainhandHitsDone          = mainhandHitsDone + 1

        -- Check each hit for multis, but stop after we get 2 multi procs
        if numMultiProcs < 2 then
            local extraMultis = isRanged and 0 or getMultiAttacks(attacker, target, wsParams, false, false)
            numMainHandMultis = numMainHandMultis + extraMultis
            numMultiProcs     = xi.weaponskills.nextMultiProcCount(numMultiProcs, extraMultis)
        end

        if xi.weaponskills.ammoTrackingActive(ammoCount) then
            local ammoStep = xi.weaponskills.planAmmoStep(true, ammoUsed, ammoCount, useAmmo(attacker))
            ammoUsed = ammoStep.ammoUsed
            if ammoStep.forceStop then
                hitsDone = 8 -- Attack while loops will stop if hitsDone is 8 or higher
            end
        end
    end

    -- Proc any mainhand multi attacks.
    local mainhandMultiHitsDone = 0

    while mainhandMultiHitsDone < numMainHandMultis and xi.weaponskills.wsHitLoopContinues(hitsDone, finaldmg, targetHp) do
        hitdmg, calcParams    = getSingleHitDamage(attacker, target, dmg, ftp, wsParams, calcParams)

        if calcParams.melee then
            hitdmg = modifyMeleeHitDamage(attacker, target, calcParams.attackInfo, wsParams, hitdmg)
        end

        local multiPlan = xi.weaponskills.planSubsequentHitLanded(
            hitdmg,
            isJump,
            false,
            calcParams.extraOffhandHit,
            hitsDone,
            false,
            false
        )
        if multiPlan.applies then
            attacker:trySkillUp(calcParams.skillType, targetLvl)

            -- When dual wielding, the mainhand appears to count the second hit as a TP hit unless it's a 1 hit WS where the offhand will gain TP
            -- Needs better verification, in this case (1 hit ws with multis)  a DA/TA/QA may not count as TP hit and we'd move this into the offhand hit proc.
            -- Either way, this won't "cheat" players out of TP in the current implementation.
            calcParams.tpHitsLanded, calcParams.mainHitsLanded, calcParams.offhandHitsLanded =
                xi.weaponskills.applyHitTPClass(
                    multiPlan.tpClass,
                    calcParams.tpHitsLanded,
                    calcParams.mainHitsLanded,
                    calcParams.offhandHitsLanded
                )

            if multiPlan.addJumpTP then
                attacker:addTP(xi.weaponskills.jumpTPGain(mainhandTPGain, attackerTPMult))
            end
        end

        finaldmg                  = finaldmg + hitdmg
        hitsDone                  = hitsDone + 1
        mainhandMultiHitsDone     = mainhandMultiHitsDone + 1

        if xi.weaponskills.ammoTrackingActive(ammoCount) then
            local ammoStep = xi.weaponskills.planAmmoStep(true, ammoUsed, ammoCount, useAmmo(attacker))
            ammoUsed = ammoStep.ammoUsed
            if ammoStep.forceStop then
                hitsDone = 8 -- Attack while loops will stop if hitsDone is 8 or higher
            end
        end
    end

    local originalSlot = calcParams.attackInfo.slot

    -- Update params for accuracy cap/pdif purposes
    if calcParams.extraOffhandHit then
        calcParams.attackInfo.slot       = xi.slot.SUB
        calcParams.attackInfo.weaponType = offhandSkill

        if wsParams.critVaries then -- Update crit rate if this applies
            calcParams.critRate = xi.combat.physical.calculateSwingCriticalRate(attacker, target, tp, calcParams.attackInfo.slot, wsParams.critVaries)
        end
    end

    -- Recalculate hitRate with offhand acc
    -- No more damage is being processed with mainhand acc, so this is ok
    calcParams.hitRate = xi.weaponskills.getHitRate(attacker, target, calcParams.bonusAcc, xi.attackAnimation.LEFT_ATTACK)

    -- Do the extra hit for our offhand if applicable
    if calcParams.extraOffhandHit and xi.weaponskills.wsHitLoopContinues(hitsDone, finaldmg, targetHp) then
        local offhandDmg      = xi.weaponskills.offhandBaseDamage(calcParams.weaponDamage[2], calcParams.fSTR, wsc, alpha)
        hitdmg, calcParams    = getSingleHitDamage(attacker, target, offhandDmg, ftp, wsParams, calcParams)

        if calcParams.melee then
            hitdmg = modifyMeleeHitDamage(attacker, target, calcParams.attackInfo, wsParams, hitdmg)
        end

        local offPlan = xi.weaponskills.planSubsequentHitLanded(
            hitdmg,
            isJump,
            false,
            true,
            hitsDone,
            false,
            true
        )
        if offPlan.applies then
            attacker:trySkillUp(offhandSkill, targetLvl)

            -- If this is the second swing of the WS (1 hit ws) the offhand appears to count for TP gain
            -- Needs better verification
            calcParams.tpHitsLanded, calcParams.mainHitsLanded, calcParams.offhandHitsLanded =
                xi.weaponskills.applyHitTPClass(
                    offPlan.tpClass,
                    calcParams.tpHitsLanded,
                    calcParams.mainHitsLanded,
                    calcParams.offhandHitsLanded
                )

            if offPlan.addJumpTP then
                attacker:addTP(xi.weaponskills.jumpTPGain(subTPGain, attackerTPMult))
            end
        end

        finaldmg = finaldmg + hitdmg
        hitsDone = hitsDone + 1

        numOffhandMultis = getMultiAttacks(attacker, target, wsParams, false, true)
        numMultiProcs    = xi.weaponskills.nextMultiProcCount(numMultiProcs, numOffhandMultis)
    end

    -- Proc any offhand multi attacks.
    local offhandMultiHitsDone = 0

    while offhandMultiHitsDone < numOffhandMultis and xi.weaponskills.wsHitLoopContinues(hitsDone, finaldmg, targetHp) do
        local offhandDmg      = xi.weaponskills.offhandBaseDamage(calcParams.weaponDamage[2], calcParams.fSTR, wsc, alpha)
        hitdmg, calcParams    = getSingleHitDamage(attacker, target, offhandDmg, ftp, wsParams, calcParams)

        if calcParams.melee then
            hitdmg = modifyMeleeHitDamage(attacker, target, calcParams.attackInfo, wsParams, hitdmg)
        end

        -- Offhand multi-attacks always count as offhand hits when landed (no classify path upstream).
        local offMultiPlan = xi.weaponskills.planOffhandMultiLanded(hitdmg, isJump)
        if offMultiPlan.applies then
            attacker:trySkillUp(offhandSkill, targetLvl)

            if offMultiPlan.addJumpTP then
                attacker:addTP(xi.weaponskills.jumpTPGain(subTPGain, attackerTPMult))
            end

            calcParams.offhandHitsLanded = calcParams.offhandHitsLanded + 1
        end

        finaldmg             = finaldmg + hitdmg
        hitsDone             = hitsDone + 1
        offhandMultiHitsDone = offhandMultiHitsDone + 1
    end

    calcParams.extraHitsLanded = xi.weaponskills.extraHitsLanded(
        calcParams.mainHitsLanded,
        calcParams.offhandHitsLanded
    )

    -- Reset slot info (A listener eventually uses this, and the change to SLOT_SUB on DW will be unexpected)
    calcParams.attackInfo.slot = originalSlot

    -- Factor in "all hits" bonus damage mods
    -- TODO: does this apply to every hit of a multi hit WS as it's coming in to account for potentially excess damage here?
    finaldmg = xi.weaponskills.allHitsWSDProduct(
        finaldmg,
        isJump and 0 or attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
        isJump and 0 or attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID),
        attacker:isPet(),
        firstHitBonus,
        isJump
    )

    -- Return our raw damage to then be modified by enemy reductions based off of melee/ranged
    calcParams.finalDmg = finaldmg
    calcParams.ammoUsed = ammoUsed

    return calcParams
end

-- Sets up the necessary calcParams for a melee WS before passing it to calculateRawWSDmg. When the raw
-- damage is returned, handles reductions based on target resistances and passes off to xi.weaponskills.takeWeaponskillDamage.
-- Pure gearAcc from gear fTP: ceil(gearFTP * 100). TODO: separate gear fTP and acc.
xi.weaponskills.gearAccFromFTP = function(gearFTP)
    return math.ceil(gearFTP * 100)
end

-- Pure mustMiss for physical WS: Perfect Dodge, or ALL_MISS unless hitsHigh.
xi.weaponskills.physicalMustMiss = function(hasPerfectDodge, hasAllMiss, hitsHigh)
    return hasPerfectDodge or (hasAllMiss and not hitsHigh)
end

-- Pure Sneak Attack applicability: has SA and (behind or Hide or target Doubt).
xi.weaponskills.sneakApplicable = function(hasSneakAttack, isBehind, hasHide, targetHasDoubt)
    return hasSneakAttack and (isBehind or hasHide or targetHasDoubt)
end

-- Pure Trick Attack applicability: taChar is present.
xi.weaponskills.trickApplicable = function(hasTaChar)
    return hasTaChar
end

-- Pure Assassin trait gate for forced first crit on TA.
xi.weaponskills.assassinApplicable = function(trickApplicable, hasAssassinTrait)
    return trickApplicable and hasAssassinTrait
end

-- Pure guaranteed hit: SA or TA applicable.
xi.weaponskills.guaranteedHit = function(sneakApplicable, trickApplicable)
    return sneakApplicable or trickApplicable
end

-- Pure forced first crit: SA or Assassin+TA.
xi.weaponskills.forcedFirstCrit = function(sneakApplicable, assassinApplicable)
    return sneakApplicable or assassinApplicable
end

-- Pure Jump vs normal bonus fTP / Acc / WSmods injects.
-- Returns bonusfTP, bonusAcc, bonusWSmods.
xi.weaponskills.physicalBonusInjects = function(isJump, gearFTP, gearAcc, jumpAccBonus, wsAccMod, bonusWSmods)
    if isJump then
        return 0, jumpAccBonus, 0
    end

    return gearFTP, gearAcc + wsAccMod, bonusWSmods or 0
end

-- Pure accVaries add onto bonusAcc once fTP(tp, accVaries) is resolved.
xi.weaponskills.bonusAccWithVaries = function(bonusAcc, accVariesFTP)
    return bonusAcc + accVariesFTP
end

-----------------------------------
-- Pure: doPhysicalWeaponskill setup product (slice 6775 / 6664)
-- Parity: internal/wsformula PhysicalWeaponskillSetup
-----------------------------------
-- params: hasPerfectDodge, hasAllMiss, hitsHigh,
--   hasSneakAttack, isBehind, hasHide, targetHasDoubt,
--   hasTaChar, hasAssassinTrait,
--   isJump, gearFTP, gearAcc, jumpAccBonus, wsAccMod, bonusWSmods,
--   hasAccVaries, accVariesFTP
-- returns table of calcParams flags/bonuses + firstHitAccBonus
xi.weaponskills.physicalWeaponskillSetupFromParams = function(params)
    params = params or {}
    -- Coerce injects to booleans so nil status reads become false (not nil).
    local mustMiss = not not xi.weaponskills.physicalMustMiss(
        not not params.hasPerfectDodge, not not params.hasAllMiss, not not params.hitsHigh)
    local sneak = not not xi.weaponskills.sneakApplicable(
        not not params.hasSneakAttack, not not params.isBehind,
        not not params.hasHide, not not params.targetHasDoubt)
    local trick = not not xi.weaponskills.trickApplicable(not not params.hasTaChar)
    local assassin = not not xi.weaponskills.assassinApplicable(
        trick, not not params.hasAssassinTrait)
    local bonusFTP, bonusAcc, bonusWSmods = xi.weaponskills.physicalBonusInjects(
        not not params.isJump,
        params.gearFTP or 0,
        params.gearAcc or 0,
        params.jumpAccBonus or 0,
        params.wsAccMod or 0,
        params.bonusWSmods or 0
    )
    if params.hasAccVaries then
        bonusAcc = xi.weaponskills.bonusAccWithVaries(bonusAcc, params.accVariesFTP or 0)
    end

    return {
        mustMiss           = mustMiss,
        sneakApplicable    = sneak,
        trickApplicable    = trick,
        assassinApplicable = assassin,
        guaranteedHit      = not not xi.weaponskills.guaranteedHit(sneak, trick),
        forcedFirstCrit    = not not xi.weaponskills.forcedFirstCrit(sneak, assassin),
        bonusFTP           = bonusFTP,
        bonusAcc           = bonusAcc,
        bonusWSmods        = bonusWSmods,
        firstHitAccBonus   = xi.weaponskills.firstHitAccuracyBonus(bonusAcc),
    }
end

-----------------------------------
-- Pure: doRangedWeaponskill bonus setup product (slice 6776 / 6664)
-- Parity: internal/wsformula RangedWeaponskillSetup
-----------------------------------
-- params: gearFTP, gearAcc, wsAccMod, bonusWSmods,
--   hasAccVaries, accVariesFTP,
--   hasRangedAccuracyBonus, rangedAccuracyBonus
xi.weaponskills.rangedWeaponskillSetupFromParams = function(params)
    params = params or {}
    local _, bonusAcc, bonusWSmods = xi.weaponskills.physicalBonusInjects(
        false,
        params.gearFTP or 0,
        params.gearAcc or 0,
        0,
        params.wsAccMod or 0,
        params.bonusWSmods or 0
    )
    if params.hasAccVaries then
        bonusAcc = xi.weaponskills.bonusAccWithVaries(bonusAcc, params.accVariesFTP or 0)
    end

    local firstHitBonusAcc = bonusAcc
    if params.hasRangedAccuracyBonus then
        firstHitBonusAcc = bonusAcc + (params.rangedAccuracyBonus or 0)
    end

    return {
        bonusFTP             = params.gearFTP or 0,
        bonusAcc             = bonusAcc,
        bonusWSmods          = bonusWSmods,
        firstHitBonusAcc     = firstHitBonusAcc,
        useFirstHitBonusAcc  = not not params.hasRangedAccuracyBonus,
    }
end

-- Pure hybrid magic add gate: hybridWS and target still has HP after physical.
xi.weaponskills.hybridMagicApplies = function(hybridWS, targetHP, physicalFinalDmg)
    return hybridWS and targetHP > physicalFinalDmg
end

-- Pure server weaponskill power multiply.
xi.weaponskills.applyWeaponSkillPower = function(finalDmg, weaponSkillPower)
    return finalDmg * weaponSkillPower
end

-- Pure ranged post-raw reduction product once rangedDmgTaken is injected:
--   dmg * (1 + PIERCE_SDT/10000), then floor.
xi.weaponskills.rangedWeaponskillMitigation = function(rangedDmgTakenResult, pierceSDT)
    return math.floor(rangedDmgTakenResult * (1 + pierceSDT / 10000))
end

-----------------------------------
-- Pure: physical/ranged WS final damage product (slice 6774)
-- Parity: internal/wsformula PhysicalWeaponskillFinal / RangedWeaponskillFinal
-----------------------------------
-- params: rawFinalDmg, useHybridMagic, hybridMagicDmg, weaponSkillPower
xi.weaponskills.physicalWeaponskillFinalFromParams = function(params)
    params = params or {}
    local dmg = xi.weaponskills.floorRawFinalDmg(params.rawFinalDmg or 0)
    if params.useHybridMagic then
        dmg = xi.weaponskills.hybridMagicAdd(dmg, params.hybridMagicDmg or 0)
    end

    return xi.weaponskills.applyWeaponSkillPower(dmg, params.weaponSkillPower or 1)
end

-- params: rangedDmgTakenResult, pierceSDT, useHybridMagic, hybridMagicDmg, weaponSkillPower
xi.weaponskills.rangedWeaponskillFinalFromParams = function(params)
    params = params or {}
    local dmg = xi.weaponskills.rangedWeaponskillMitigation(
        params.rangedDmgTakenResult or 0, params.pierceSDT or 0)
    if params.useHybridMagic then
        dmg = xi.weaponskills.hybridMagicAdd(dmg, params.hybridMagicDmg or 0)
    end

    return xi.weaponskills.applyWeaponSkillPower(dmg, params.weaponSkillPower or 1)
end

-- Pure residual doMagicWeaponskill / post-raw bookkeeping (OmegaXI slice 6672).

xi.weaponskills.magicWSHitsLanded      = 1
xi.weaponskills.magicWSTpHitsLanded    = 1
xi.weaponskills.magicWSExtraHitsLanded = 0

-- MAIN unless archery/marksmanship magic WS.
xi.weaponskills.magicWSAttackSlot = function(skill)
    if xi.weaponskills.isRangedMagicWeaponskill(skill) then
        return xi.slot.RANGED
    end

    return xi.slot.MAIN
end

xi.weaponskills.magicWSDStatIsCHR = function(dStatIsCHR)
    return dStatIsCHR
end

-- gearAccFromFTP + WSACC (magic path stacks WSACC here).
xi.weaponskills.magicWSBonusAcc = function(gearAccFromFTP, wsAccMod)
    return gearAccFromFTP + wsAccMod
end

xi.weaponskills.magicWSScarletProduct = function(dmg, scarletMult)
    return dmg * scarletMult
end

xi.weaponskills.magicWSNegativeEarlyReturn = function(dmg)
    return dmg < 0
end

xi.weaponskills.planMagicWSShadowAbsorb = function()
    return { shadowsAbsorbed = 1, damage = 0, trySkillUp = false }
end

xi.weaponskills.magicWSSkillUpApplies = function(dmg)
    return dmg > 0
end

xi.weaponskills.floorRawFinalDmg = function(finalDmg)
    return math.floor(finalDmg)
end

xi.weaponskills.hybridMagicAdd = function(physicalFinal, hybridMagicDmg)
    return physicalFinal + hybridMagicDmg
end

xi.weaponskills.shouldRemoveAmmo = function(ammoUsed)
    return ammoUsed > 0
end

xi.weaponskills.doPhysicalWeaponskill = function(attacker, target, wsID, wsParams, tp, action, primaryMsg, taChar)
    -- Set up conditions and wsParams used for calculating weaponskill damage
    local gearFTP = xi.combat.physical.calculateFTPBonus(attacker)
    local gearAcc = xi.weaponskills.gearAccFromFTP(gearFTP) -- TODO: Separate gear fTP and acc bonuses
    local attack =
    {
        ['type']       = xi.attackType.PHYSICAL,
        ['slot']       = xi.slot.MAIN,
        ['weaponType'] = attacker:getWeaponSkillType(xi.slot.MAIN),
        ['damageType'] = attacker:getWeaponDamageType(xi.slot.MAIN),
    }

    local calcParams = {}
    calcParams.wsID                    = wsID
    calcParams.attackInfo              = attack
    calcParams.weaponDamage            = xi.weaponskills.getMeleeDmg(attacker, attack.weaponType, wsParams.kick)
    calcParams.fSTR                    = xi.combat.physical.calculateMeleeStatFactor(attacker, target)
    calcParams.accStat                 = attacker:getACC()
    calcParams.melee                   = true
    calcParams.taChar                  = taChar
    calcParams.mightyStrikesApplicable = attacker:hasStatusEffect(xi.effect.MIGHTY_STRIKES)
    calcParams.extraOffhandHit         = attacker:isDualWielding()
    calcParams.hybridHit               = wsParams.hybridWS
    calcParams.flourishEffect          = attacker:getStatusEffect(xi.effect.BUILDING_FLOURISH)
    calcParams.bonusTP                 = wsParams.bonusTP or 0
    calcParams.tpUsed                  = tp
    calcParams.attackType              = xi.attackType.PHYSICAL

    -- Pure setup product for miss/SA/TA/guaranteed/bonus injects (slice 6775).
    local isJump = wsParams.isJump or false
    local setup = xi.weaponskills.physicalWeaponskillSetupFromParams({
        hasPerfectDodge  = target:hasStatusEffect(xi.effect.PERFECT_DODGE),
        hasAllMiss       = target:hasStatusEffect(xi.effect.ALL_MISS),
        hitsHigh         = wsParams.hitsHigh,
        hasSneakAttack   = attacker:hasStatusEffect(xi.effect.SNEAK_ATTACK),
        isBehind         = attacker:isBehind(target),
        hasHide          = attacker:hasStatusEffect(xi.effect.HIDE),
        targetHasDoubt   = target:hasStatusEffect(xi.effect.DOUBT),
        hasTaChar        = taChar ~= nil,
        hasAssassinTrait = attacker:hasTrait(xi.trait.ASSASSIN),
        isJump           = isJump,
        gearFTP          = gearFTP,
        gearAcc          = gearAcc,
        jumpAccBonus     = attacker:getMod(xi.mod.JUMP_ACC_BONUS),
        wsAccMod         = attacker:getMod(xi.mod.WSACC),
        bonusWSmods      = wsParams.bonusWSmods,
        hasAccVaries     = wsParams.accVaries ~= nil,
        accVariesFTP     = wsParams.accVaries and xi.weaponskills.fTP(tp, wsParams.accVaries) or 0,
    })
    calcParams.mustMiss           = setup.mustMiss
    calcParams.sneakApplicable    = setup.sneakApplicable
    calcParams.trickApplicable    = setup.trickApplicable
    calcParams.assassinApplicable = setup.assassinApplicable
    calcParams.guaranteedHit      = setup.guaranteedHit
    calcParams.forcedFirstCrit    = setup.forcedFirstCrit
    calcParams.bonusfTP           = setup.bonusFTP
    calcParams.bonusAcc           = setup.bonusAcc
    calcParams.bonusWSmods        = setup.bonusWSmods

    calcParams.firstHitRate = xi.weaponskills.getHitRate(attacker, target, setup.firstHitAccBonus, xi.attackAnimation.RIGHT_ATTACK)
    calcParams.hitRate      = xi.weaponskills.getHitRate(attacker, target, calcParams.bonusAcc, xi.attackAnimation.RIGHT_ATTACK)
    calcParams.skillType    = attack.weaponType

    -- Send our wsParams off to calculate our raw WS damage, hits landed, and shadows absorbed
    calcParams = xi.weaponskills.calculateRawWSDmg(attacker, target, wsID, tp, action, wsParams, calcParams)

    -- Hybrid gate needs floored physical final as HP threshold / hybrid base.
    local flooredPhysical = xi.weaponskills.floorRawFinalDmg(calcParams.finalDmg)
    local hybridMagicDmg  = 0
    local useHybrid       = false
    if xi.weaponskills.hybridMagicApplies(wsParams.hybridWS, target:getHP(), flooredPhysical) then
        hybridMagicDmg = calculateHybridMagicDamage(tp, flooredPhysical, attacker, target, wsParams, calcParams, wsID)
        useHybrid      = true
    end

    -- Delete statuses that may have been spent by the WS
    attacker:delStatusEffectsByFlag(xi.effectFlag.DETECTABLE)
    attacker:delStatusEffect(xi.effect.SNEAK_ATTACK)
    attacker:delStatusEffectSilent(xi.effect.BUILDING_FLOURISH)

    -- Pure final product: floor → hybrid? → WEAPON_SKILL_POWER (slice 6774).
    local finaldmg = xi.weaponskills.physicalWeaponskillFinalFromParams({
        rawFinalDmg      = calcParams.finalDmg,
        useHybridMagic   = useHybrid,
        hybridMagicDmg   = hybridMagicDmg,
        weaponSkillPower = xi.settings.main.WEAPON_SKILL_POWER,
    })
    calcParams.finalDmg = finaldmg
    finaldmg            = xi.weaponskills.takeWeaponskillDamage(target, attacker, wsParams, primaryMsg, attack, calcParams, action)

    return finaldmg, calcParams.criticalHit, calcParams.tpHitsLanded, calcParams.extraHitsLanded, calcParams.shadowsAbsorbed
end

-- Sets up the necessary calcParams for a ranged WS before passing it to calculateRawWSDmg. When the raw
-- damage is returned, handles reductions based on target resistances and passes off to xi.weaponskills.takeWeaponskillDamage.
xi.weaponskills.doRangedWeaponskill = function(attacker, target, wsID, wsParams, tp, action, primaryMsg)
    -- Set up conditions and params used for calculating weaponskill damage
    local gearFTP = xi.combat.physical.calculateFTPBonus(attacker)
    local gearAcc = xi.weaponskills.gearAccFromFTP(gearFTP) -- TODO: Separate gear fTP and acc bonuses

    local attack =
    {
        ['type']       = xi.attackType.RANGED,
        ['slot']       = xi.slot.RANGED,
        ['weaponType'] = attacker:getWeaponSkillType(xi.slot.RANGED),
        ['damageType'] = attacker:getWeaponDamageType(xi.slot.RANGED),
    }

    -- Pure bonus setup product (slice 6776). Host residual: getRangedHitRate.
    local setup = xi.weaponskills.rangedWeaponskillSetupFromParams({
        gearFTP                = gearFTP,
        gearAcc                = gearAcc,
        wsAccMod               = attacker:getMod(xi.mod.WSACC),
        bonusWSmods            = wsParams.bonusWSmods,
        hasAccVaries           = wsParams.accVaries ~= nil,
        accVariesFTP           = wsParams.accVaries and xi.weaponskills.fTP(tp, wsParams.accVaries) or 0,
        hasRangedAccuracyBonus = wsParams.rangedAccuracyBonus ~= nil,
        rangedAccuracyBonus    = wsParams.rangedAccuracyBonus or 0,
    })

    local calcParams =
    {
        wsID                    = wsID,
        attackInfo              = attack,
        weaponDamage            = { attacker:getRangedDmg() },
        skillType               = attacker:getWeaponSkillType(xi.slot.RANGED),
        fSTR                    = xi.combat.physical.calculateRangedStatFactor(attacker, target),
        accStat                 = attacker:getRACC(),
        melee                   = false,
        mustMiss                = false,
        sneakApplicable         = false,
        trickApplicable         = false,
        assassinApplicable      = false,
        mightyStrikesApplicable = false,
        forcedFirstCrit         = false,
        extraOffhandHit         = false,
        flourishEffect          = false,
        tpUsed                  = tp,
        bonusTP                 = wsParams.bonusTP or 0,
        bonusfTP                = setup.bonusFTP,
        bonusAcc                = setup.bonusAcc,
        bonusWSmods             = setup.bonusWSmods,
        attackType              = xi.attackType.RANGED,
    }

    -- Split Shot/Piercing Arrow and Empyreal Arrow/Detonator are confirmed for this. Theoretically Last Stand could have a bonus too, and if so it would likely be first hit only.
    if setup.useFirstHitBonusAcc then
        calcParams.firstHitRate = xi.weaponskills.getRangedHitRate(attacker, target, setup.firstHitBonusAcc)
    end

    calcParams.hitRate = xi.weaponskills.getRangedHitRate(attacker, target, calcParams.bonusAcc)

    -- Send our params off to calculate our raw WS damage, hits landed, and shadows absorbed
    calcParams = xi.weaponskills.calculateRawWSDmg(attacker, target, wsID, tp, action, wsParams, calcParams)

    -- Delete statuses that may have been spent by the WS
    attacker:delStatusEffectsByFlag(xi.effectFlag.DETECTABLE)
    attacker:delStatusEffect(xi.effect.FLASHY_SHOT)
    attacker:delStatusEffect(xi.effect.STEALTH_SHOT)

    -- Host residual: rangedDmgTaken + PIERCE_SDT injects for pure mitigation.
    local rangedTaken = target:rangedDmgTaken(calcParams.finalDmg)
    local pierceSDT   = target:getMod(xi.mod.PIERCE_SDT)
    local mitigated   = xi.weaponskills.rangedWeaponskillMitigation(rangedTaken, pierceSDT)

    local hybridMagicDmg = 0
    local useHybrid      = false
    if xi.weaponskills.hybridMagicApplies(wsParams.hybridWS, target:getHP(), mitigated) then
        hybridMagicDmg = calculateHybridMagicDamage(tp, mitigated, attacker, target, wsParams, calcParams, wsID)
        useHybrid      = true
    end

    -- Pure final product: mitigation → hybrid? → WEAPON_SKILL_POWER (slice 6774).
    local finaldmg = xi.weaponskills.rangedWeaponskillFinalFromParams({
        rangedDmgTakenResult = rangedTaken,
        pierceSDT            = pierceSDT,
        useHybridMagic       = useHybrid,
        hybridMagicDmg       = hybridMagicDmg,
        weaponSkillPower     = xi.settings.main.WEAPON_SKILL_POWER,
    })
    calcParams.finalDmg = finaldmg

    finaldmg = xi.weaponskills.takeWeaponskillDamage(target, attacker, wsParams, primaryMsg, attack, calcParams, action)

    -- Ammo needs to be removed after xi.weaponskills.takeWeaponskillDamage for delay/tp return uses
    if xi.weaponskills.shouldRemoveAmmo(calcParams.ammoUsed or 0) then
        attacker:removeAmmo(calcParams.ammoUsed)
    end

    return finaldmg, calcParams.criticalHit, calcParams.tpHitsLanded, calcParams.extraHitsLanded, calcParams.shadowsAbsorbed
end

-- params: ftpMod, wsc_str, wsc_dex, wsc_vit, wsc_agi, wsc_int, wsc_mnd, wsc_chr,
--         ele (xi.element.FIRE), skill (xi.skill.STAFF)

-- fINT component of a magic weaponskill, from the attacker's dStat minus the
-- target's INT.
--
-- The three branches differ in more than their multiplier: ranged and CHR
-- weaponskills clamp without flooring, so a CHR result can be fractional, while
-- the default branch adds a flat 8 and floors *after* clamping. The CHR bound
-- is also two orders of magnitude wider than the others.
xi.weaponskills.magicWeaponskillFint = function(isRanged, dStatIsCHR, statDelta)
    if isRanged then
        -- TODO: ranged magic WS are universal in it's (AGI-INT)*2
        return utils.clamp(statDelta * 2, -32, 32)
    elseif dStatIsCHR then
        -- TODO: unknown lower cap but on dINT it normally mirrors https://www.bg-wiki.com/ffxi/Primal_Rend
        return utils.clamp(statDelta * 1.5, -651, 651)
    end

    -- But other magic WS vary. Some don't even have a component, the general
    -- case is dINT/2 + 8
    return math.floor(utils.clamp(8 + statDelta / 2, -32, 32))
end

-- Pure magic weaponskill raw damage before Scarlet Delirium / WSD / ability
-- bonuses: (WSC + mainLvl + 2 + fINT) * (fTP + gearFTP).
xi.weaponskills.magicWeaponskillRawDamage = function(wsc, mainLvl, fint, ftp, gearFTP)
    return (wsc + mainLvl + 2 + fint) * (ftp + gearFTP)
end

-- Pure all-hits / first-hit WSD product for magic weaponskills. Per-WS WSD
-- only stacks when the mod is strictly positive *and* the attacker is not a
-- pet. First-hit uses the additive form dmg + dmg * mod/100.
xi.weaponskills.magicWeaponskillWSDProduct = function(dmg, allWSDMG, perWSWSD, isPet, firstHitWSD)
    local bonusdmg = allWSDMG

    if perWSWSD > 0 and not isPet then
        bonusdmg = bonusdmg + perWSWSD
    end

    dmg = dmg * (100 + bonusdmg) / 100
    dmg = dmg + dmg * firstHitWSD / 100

    return dmg
end

-----------------------------------
-- Pure: magic WS damage product composition (slice 6770 / 6653)
-- Parity: internal/wsformula MagicWeaponskillDamagePreSevere / Final
-----------------------------------

-- Mid-product through resist/shell floors once WSC/fINT/fTP/scarlet/WSD and
-- ability residual injects are known. Host applies handleSevereDamage next.
-- params: wsc, mainLvl, fint, ftp, gearFTP, scarletMult,
--   allWSDMG, perWSWSD, isPet, firstHitWSD,
--   afterAbility, useAfterAbility, resist, damageAdj
xi.weaponskills.magicWeaponskillDamagePreSevereFromParams = function(params)
    params = params or {}
    local dmg = xi.weaponskills.magicWeaponskillRawDamage(
        params.wsc or 0,
        params.mainLvl or 0,
        params.fint or 0,
        params.ftp or 0,
        params.gearFTP or 0
    )
    dmg = xi.weaponskills.magicWSScarletProduct(dmg, params.scarletMult or 1)
    dmg = xi.weaponskills.magicWeaponskillWSDProduct(
        dmg,
        params.allWSDMG or 0,
        params.perWSWSD or 0,
        params.isPet,
        params.firstHitWSD or 0
    )
    if params.useAfterAbility then
        dmg = params.afterAbility or 0
    end

    return xi.weaponskills.magicMitigationFloors(
        dmg, params.resist or 1, params.damageAdj or 1)
end

-- Post-severe product through WEAPON_SKILL_POWER.
-- params: afterSevere, absorb, nullify, afterMitigate, useAfterMitigate,
--   weaponSkillPower
-- returns: damage, negativeEarly
xi.weaponskills.magicWeaponskillDamageFinalFromParams = function(params)
    params = params or {}
    local afterSevere = params.afterSevere or 0
    if xi.weaponskills.magicWSNegativeEarlyReturn(afterSevere) then
        return afterSevere, true
    end

    local dmg = xi.weaponskills.magicWeaponskillAbsorbNullify(
        afterSevere,
        params.absorb or 1,
        params.nullify or 1
    )
    if params.useAfterMitigate then
        dmg = params.afterMitigate or 0
    end

    return xi.weaponskills.applyWeaponSkillPower(dmg, params.weaponSkillPower or 1), false
end

xi.weaponskills.doMagicWeaponskill = function(attacker, target, wsID, wsParams, tp, action, primaryMsg)
    -- Set up conditions and wsParams used for calculating weaponskill damage
    local attack =
    {
        ['type']       = xi.attackType.MAGICAL,
        ['slot']       = xi.weaponskills.magicWSAttackSlot(wsParams.skill or 0),
        ['weaponType'] = attacker:getWeaponSkillType(xi.slot.MAIN),
        ['damageType'] = xi.damageType.ELEMENTAL + wsParams.ele
    }

    local calcParams =
    {
        ['shadowsAbsorbed'] = 0,
        ['hitsLanded']      = xi.weaponskills.magicWSHitsLanded,
        ['tpHitsLanded']    = xi.weaponskills.magicWSTpHitsLanded,
        ['extraHitsLanded'] = xi.weaponskills.magicWSExtraHitsLanded,
        ['bonusTP']         = wsParams.bonusTP or 0,
        ['wsID']            = wsID,
    }

    local dStat   = wsParams.dStat and wsParams.dStat or xi.mod.INT
    local gearFTP = xi.combat.physical.calculateFTPBonus(attacker)
    local gearAcc = xi.weaponskills.magicWSBonusAcc(
        xi.weaponskills.gearAccFromFTP(gearFTP),
        attacker:getMod(xi.mod.WSACC)
    ) -- TODO: Separate gear fTP and acc bonuses
    local fint    = 0
    local dmg     = 0

    -- TODO: ranged magic WS are universal in it's (AGI-INT)*2
    -- But other magic WS vary. Some don't even have a component, the general case is dINT/2 + 8
    fint = xi.weaponskills.magicWeaponskillFint(
        attack.slot == xi.slot.RANGED,
        xi.weaponskills.magicWSDStatIsCHR(dStat == xi.mod.CHR),
        attacker:getStat(dStat) - target:getStat(xi.mod.INT)
    )

    -- Magic-based WSes never miss, so we don't need to worry about calculating a miss, only if a shadow absorbed it.
    -- Host residual: WSC/entity mods, ability bonuses, resist, severe, absorb, Phalanx/OFA/Stoneskin.
    -- Pure product: magicWeaponskillDamage*FromParams (slice 6770).
    if not shadowAbsorb(target) then
        local wsc = xi.combat.physical.calculateWSC(attacker, wsParams.str_wsc, wsParams.dex_wsc, wsParams.vit_wsc, wsParams.agi_wsc, wsParams.int_wsc, wsParams.mnd_wsc, wsParams.chr_wsc)
        local ftp = xi.weaponskills.fTP(tp, wsParams.ftpMod)

        -- Build scarlet+WSD product so ability bonuses see the same intermediate.
        local preAbility = xi.weaponskills.magicWeaponskillRawDamage(wsc, attacker:getMainLvl(), fint, ftp, gearFTP)
        preAbility = xi.weaponskills.magicWSScarletProduct(
            preAbility, xi.combat.damage.scarletDeliriumMultiplier(attacker))
        preAbility = xi.weaponskills.magicWeaponskillWSDProduct(
            preAbility,
            attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
            attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID),
            attacker:isPet(),
            attacker:getMod(xi.mod.ALL_WSDMG_FIRST_HIT)
        )

        local afterAbility = math.floor(addBonusesAbility(attacker, wsParams.ele, target, preAbility, wsParams))
        local resist = xi.combat.magicHitRate.calculateResistRate(
            attacker, target, 0, wsParams.skill, 0, wsParams.ele, 0, 0, gearAcc)
        local damageAdj = xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false)

        local preSevere = xi.weaponskills.magicWeaponskillDamagePreSevereFromParams({
            wsc             = wsc,
            mainLvl         = attacker:getMainLvl(),
            fint            = fint,
            ftp             = ftp,
            gearFTP         = gearFTP,
            scarletMult     = xi.combat.damage.scarletDeliriumMultiplier(attacker),
            allWSDMG        = attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
            perWSWSD        = attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID),
            isPet           = attacker:isPet(),
            firstHitWSD     = attacker:getMod(xi.mod.ALL_WSDMG_FIRST_HIT),
            useAfterAbility = true,
            afterAbility    = afterAbility,
            resist          = resist,
            damageAdj       = damageAdj,
        })

        local afterSevere = math.floor(target:handleSevereDamage(preSevere, false))

        local absorb  = 1
        local nullify = 1
        if not xi.weaponskills.magicWSNegativeEarlyReturn(afterSevere) then
            absorb  = xi.spells.damage.calculateAbsorption(target, wsParams.ele, true)
            nullify = xi.spells.damage.calculateNullification(target, wsParams.ele, true, false)
        end

        local afterAbsorb = xi.weaponskills.magicWeaponskillAbsorbNullify(afterSevere, absorb, nullify)
        local afterMitigate = afterAbsorb
        local useAfterMitigate = false
        if not xi.weaponskills.magicWSNegativeEarlyReturn(afterSevere) then
            afterMitigate = utils.handlePhalanx(target, afterAbsorb)
            afterMitigate = utils.handleOneForAll(target, afterMitigate)
            afterMitigate = utils.handleStoneskin(target, afterMitigate)
            useAfterMitigate = true
        end

        local negativeEarly
        dmg, negativeEarly = xi.weaponskills.magicWeaponskillDamageFinalFromParams({
            afterSevere      = afterSevere,
            absorb           = absorb,
            nullify          = nullify,
            afterMitigate    = afterMitigate,
            useAfterMitigate = useAfterMitigate,
            weaponSkillPower = xi.settings.main.WEAPON_SKILL_POWER,
        })

        if negativeEarly then
            calcParams.finalDmg = dmg
            dmg = xi.weaponskills.takeWeaponskillDamage(target, attacker, wsParams, primaryMsg, attack, calcParams, action)
            return dmg
        end
    else
        local shadowPlan = xi.weaponskills.planMagicWSShadowAbsorb()
        calcParams.shadowsAbsorbed = shadowPlan.shadowsAbsorbed
        dmg = shadowPlan.damage
    end

    calcParams.finalDmg = dmg

    if xi.weaponskills.magicWSSkillUpApplies(dmg) then
        attacker:trySkillUp(attack.weaponType, target:getMainLvl())
    end

    dmg = xi.weaponskills.takeWeaponskillDamage(target, attacker, wsParams, primaryMsg, attack, calcParams, action)

    return dmg, calcParams.criticalHit, calcParams.tpHitsLanded, calcParams.extraHitsLanded, calcParams.shadowsAbsorbed
end

-- After WS damage is calculated and damage reduction has been taken into account by the calling function,
-- handles displaying the appropriate action/message, delivering the damage to the mob, and any enmity from it
-- Pure action-message plan for takeWeaponskillDamage once hit/shadow tallies
-- and final damage are known. Host residual: action:messageID/param/resolution.
--
-- Pin: hitsLanded > 0 and finalDmg == 0 still sets DAMAGE/DAMAGE_SECONDARY but
-- does *not* set a resolution.
xi.weaponskills.takeWeaponskillDamagePlan = function(hitsLanded, finalDmg, shadowsAbsorbed, primaryMsg, guardedHits)
    local plan =
    {
        message        = 0,
        setParam       = false,
        param          = 0,
        setResolution  = false,
        resolution     = 0,
    }

    if hitsLanded > 0 then
        if finalDmg >= 0 then
            plan.message = primaryMsg and xi.msg.basic.DAMAGE or xi.msg.basic.DAMAGE_SECONDARY

            if finalDmg > 0 then
                plan.setResolution = true
                plan.resolution    = (guardedHits and guardedHits > 0) and xi.action.resolution.GUARD or xi.action.resolution.HIT
            end
        else
            plan.message = primaryMsg and xi.msg.basic.SELF_HEAL or xi.msg.basic.SELF_HEAL_SECONDARY
        end
    elseif shadowsAbsorbed > 0 then
        plan.message       = xi.msg.basic.SHADOW_ABSORB
        plan.setParam      = true
        plan.param         = shadowsAbsorbed
        plan.setResolution = true
        plan.resolution    = xi.action.resolution.MISS
    else
        plan.message       = primaryMsg and xi.msg.basic.SKILL_MISS or xi.msg.basic.EVADES
        plan.setResolution = true
        plan.resolution    = xi.action.resolution.MISS
    end

    return plan
end

-- Jump skills zero attacker TP mult and wipe extraHitsLanded so multi-hit
-- bonuses do not feed TP from takeWeaponskillDamage.
xi.weaponskills.jumpTPAdjust = function(isJump, attackerTPMult, extraHitsLanded)
    if isJump then
        return 0, 0
    end

    return attackerTPMult, extraHitsLanded
end

-- Store TP modifier: 1 + STORETP / 100 (inhibit TP not applied here).
xi.weaponskills.storeTPModifier = function(storeTPMod)
    return 1 + storeTPMod / 100
end

-- Extra-hit TP bonus passed into takeWeaponskillDamage core:
--   extraHitsLanded * 10 * storeTPModifier + bonusTP
xi.weaponskills.weaponskillExtraHitTP = function(extraHitsLanded, storeTPModifier, bonusTP)
    return (extraHitsLanded * 10 * storeTPModifier) + bonusTP
end

-- Pure enmity-source selection: Trick Attack char when present, else attacker.
xi.weaponskills.weaponskillEnmityUsesOverride = function(overrideCE, overrideVE, tpHitsLanded, extraHitsLanded)
    return overrideCE ~= nil and overrideVE ~= nil and (tpHitsLanded + extraHitsLanded > 0)
end

-- Pure residual takeWeaponskillDamage bookkeeping (OmegaXI slice 6673).

xi.weaponskills.defaultMultOr = function(has, mult)
    if not has then
        return 1
    end

    return mult
end

xi.weaponskills.shouldRecordWSDamage = function(tpHitsLanded, extraHitsLanded)
    return tpHitsLanded + extraHitsLanded > 0
end

xi.weaponskills.enmityEntityUsesTA = function(hasTaChar)
    return hasTaChar
end

xi.weaponskills.enmityFromDamageAmount = function(finalDmg, enmityMult)
    return finalDmg * enmityMult
end

xi.weaponskills.planSengikori = function(tpHitsLanded, extraHitsLanded, hasSengikori, effectPower, sengikoriBonus, targetHasSkillchain)
    if tpHitsLanded + extraHitsLanded <= 0 or not hasSengikori then
        return { applies = false, power = 0, applyMBDebuff = false, delSengikori = false }
    end

    return {
        applies       = true,
        power         = effectPower + sengikoriBonus,
        applyMBDebuff = targetHasSkillchain,
        delSengikori  = true,
    }
end

xi.weaponskills.shouldSetWeaponskillHitVar = function(finalDmg)
    return finalDmg > 0
end

xi.weaponskills.weaponskillHitLocalVar = function(wsID, finalDmg)
    return bit.lshift(wsID, 24) + finalDmg
end

xi.weaponskills.tpHitsForCore = function(tpHitsLanded, attackerTPMult)
    return tpHitsLanded * attackerTPMult
end

xi.weaponskills.absFinalDamage = function(finalDmg)
    return math.abs(finalDmg)
end

-----------------------------------
-- Pure: takeWeaponskillDamage post-entity residual product (slice 6773 / 6673)
-- Parity: internal/wsformula TakeWeaponskillPostDamage
-----------------------------------
-- params: finalDmg, tpHitsLanded, extraHitsLanded, hasTaChar,
--   hasOverrideCE, hasOverrideVE, enmityMult,
--   hasSengikori, sengikoriPower, sengikoriBonus, targetHasSkillchain, wsID
-- returns plan table with record/enmity/sengikori/hitVar fields
xi.weaponskills.takeWeaponskillPostDamageFromParams = function(params)
    params = params or {}
    local finalDmg        = params.finalDmg or 0
    local tpHitsLanded    = params.tpHitsLanded or 0
    local extraHitsLanded = params.extraHitsLanded or 0
    local hasOverride     = params.hasOverrideCE and params.hasOverrideVE

    local sengikori = xi.weaponskills.planSengikori(
        tpHitsLanded,
        extraHitsLanded,
        params.hasSengikori,
        params.sengikoriPower or 0,
        params.sengikoriBonus or 0,
        params.targetHasSkillchain
    )

    return {
        recordDamage       = xi.weaponskills.shouldRecordWSDamage(tpHitsLanded, extraHitsLanded),
        absFinalDmg        = xi.weaponskills.absFinalDamage(finalDmg),
        enmityUsesTA       = xi.weaponskills.enmityEntityUsesTA(params.hasTaChar),
        enmityUsesOverride = hasOverride and (tpHitsLanded + extraHitsLanded > 0),
        enmityAmount       = xi.weaponskills.enmityFromDamageAmount(finalDmg, params.enmityMult or 1),
        sengikori          = sengikori,
        setHitVar          = xi.weaponskills.shouldSetWeaponskillHitVar(finalDmg),
        hitLocalVar        = xi.weaponskills.weaponskillHitLocalVar(params.wsID or 0, finalDmg),
    }
end

-----------------------------------
-- Pure: takeWeaponskillDamage TP inject product (slice 6772 / 6661)
-- Parity: internal/wsformula TakeWeaponskillTPInject
-----------------------------------
-- params: isJump, attackerTPMult, extraHitsLanded, storeTPMod, bonusTP, tpHitsLanded
-- returns: { attackerTPMult, extraHitsLanded, extraHitTP, tpHitsForCore }
xi.weaponskills.takeWeaponskillTPInjectFromParams = function(params)
    params = params or {}
    local attackerTPMult, extraHitsLanded = xi.weaponskills.jumpTPAdjust(
        params.isJump,
        params.attackerTPMult or 1,
        params.extraHitsLanded or 0
    )
    local storeTPModifier = xi.weaponskills.storeTPModifier(params.storeTPMod or 0)
    local extraHitTP = xi.weaponskills.weaponskillExtraHitTP(
        extraHitsLanded, storeTPModifier, params.bonusTP or 0)

    return {
        attackerTPMult  = attackerTPMult,
        extraHitsLanded = extraHitsLanded,
        extraHitTP      = extraHitTP,
        tpHitsForCore   = xi.weaponskills.tpHitsForCore(params.tpHitsLanded or 0, attackerTPMult),
    }
end

xi.weaponskills.takeWeaponskillDamage = function(defender, attacker, wsParams, primaryMsg, attack, wsResults, action)
    local finaldmg = wsResults.finalDmg

    local plan = xi.weaponskills.takeWeaponskillDamagePlan(
        wsResults.hitsLanded,
        finaldmg,
        wsResults.shadowsAbsorbed,
        primaryMsg,
        wsResults.guardedHits
    )

    action:messageID(defender:getID(), plan.message)

    if plan.setParam then
        action:param(defender:getID(), plan.param)
    end

    if plan.setResolution then
        action:resolution(defender:getID(), plan.resolution)
    end

    local targetTPMult   = xi.weaponskills.defaultMultOr(wsParams.targetTPMult ~= nil, wsParams.targetTPMult or 1)
    local attackerTPMult = xi.weaponskills.defaultMultOr(wsParams.attackerTPMult ~= nil, wsParams.attackerTPMult or 1)
    local isJump         = wsParams.isJump or false

    -- Pure Jump/STORETP/extra-hit TP inject (slice 6772). Host residual: STORETP mod.
    local tpInject = xi.weaponskills.takeWeaponskillTPInjectFromParams({
        isJump          = isJump,
        attackerTPMult  = attackerTPMult,
        extraHitsLanded = wsResults.extraHitsLanded or 0,
        storeTPMod      = attacker:getMod(xi.mod.STORETP),
        bonusTP         = wsResults.bonusTP or 0,
        tpHitsLanded    = wsResults.tpHitsLanded or 0,
    })
    attackerTPMult              = tpInject.attackerTPMult
    wsResults.extraHitsLanded   = tpInject.extraHitsLanded
    local extraHitTP            = tpInject.extraHitTP

    finaldmg = defender:takeWeaponskillDamage(
        attacker,
        finaldmg,
        attack.type,
        attack.damageType,
        attack.slot,
        primaryMsg,
        tpInject.tpHitsForCore,
        extraHitTP,
        targetTPMult
    )

    -- Pure residual plan after entity damage (slice 6773). Host writes
    -- recordDamage / enmity / Sengikori mods / weaponskillHit local var.
    local sengikoriEffect = attacker:getStatusEffect(xi.effect.SENGIKORI)
    local post = xi.weaponskills.takeWeaponskillPostDamageFromParams({
        finalDmg            = finaldmg,
        tpHitsLanded        = wsResults.tpHitsLanded or 0,
        extraHitsLanded     = wsResults.extraHitsLanded or 0,
        hasTaChar           = wsResults.taChar ~= nil,
        hasOverrideCE       = wsParams.overrideCE ~= nil,
        hasOverrideVE       = wsParams.overrideVE ~= nil,
        enmityMult          = xi.weaponskills.defaultMultOr(wsParams.enmityMult ~= nil, wsParams.enmityMult or 1),
        hasSengikori        = sengikoriEffect ~= nil,
        sengikoriPower      = sengikoriEffect and sengikoriEffect:getPower() or 0,
        sengikoriBonus      = attacker:getMod(xi.mod.SENGIKORI_BONUS), -- Additive % bonus: https://www.ffxiah.com/forum/topic/23457/july-11-sam-update/4/#1421344
        targetHasSkillchain = defender:hasStatusEffect(xi.effect.SKILLCHAIN),
        wsID                = wsResults.wsID or 0,
    })

    if post.recordDamage then
        action:recordDamage(defender, attack.type, post.absFinalDmg, wsResults.criticalHit)
    end

    local enmityEntity = post.enmityUsesTA and wsResults.taChar or attacker
    if post.enmityUsesOverride then
        defender:addEnmity(enmityEntity, wsParams.overrideCE, wsParams.overrideVE)
    else
        defender:updateEnmityFromDamage(enmityEntity, post.enmityAmount)
    end

    -- TODO: does Sengikori not apply if you do 0 damage (or absorb)?
    -- Skillchains will still "proc" if you do 0 damage, so assume it does for now
    local sengikoriPlan = post.sengikori
    if sengikoriPlan.applies then
        -- If no SC effect, apply SC damage debuff
        -- If there is one, apply MB damage debuff
        -- This "effect" lasts until the it's used or the SC goes away
        -- see https://wiki.ffo.jp/html/20051.html
        if sengikoriPlan.applyMBDebuff then
            defender:setMod(xi.mod.SENGIKORI_MB_DMG_DEBUFF, sengikoriPlan.power)
        else
            defender:setMod(xi.mod.SENGIKORI_SC_DMG_DEBUFF, sengikoriPlan.power)
        end

        if sengikoriPlan.delSengikori then
            attacker:delStatusEffect(xi.effect.SENGIKORI)
        end
    end

    if post.setHitVar then
        -- Pack the weaponskill ID in the top 8 bits of this variable which is utilized
        -- in OnMobDeath in luautils.  Max WSID is 255.
        defender:setLocalVar('weaponskillHit', post.hitLocalVar)
    end

    return finaldmg
end

-----------------------------------
-- Pure: getMeleeDmg inject form (slice 6754 / internal/wsformula.MeleeDmg)
-- H2H/NONE: main = (kick+footwork ? kickDmg : mainhandDmg) + skill*0.11+3; off = main
-- Other skills: return injected main/off unchanged.
-----------------------------------
xi.weaponskills.h2hSkillFactor = 0.11
xi.weaponskills.h2hSkillBase   = 3.0

xi.weaponskills.h2hSkillDamageFromParams = function(params)
    params = params or {}
    return (params.skillLevel or 0) * xi.weaponskills.h2hSkillFactor + xi.weaponskills.h2hSkillBase
end

-- params: weaponType, kick, mainhandDmg, offhandDmg, h2hSkillLevel, hasFootwork, kickDmg
-- returns: main, off
xi.weaponskills.meleeDmgFromParams = function(params)
    params = params or {}
    local main = params.mainhandDmg or 0
    local off  = params.offhandDmg or 0
    local weaponType = params.weaponType or 0

    if
        weaponType == xi.skill.HAND_TO_HAND or
        weaponType == xi.skill.NONE
    then
        local h2h = xi.weaponskills.h2hSkillDamageFromParams({
            skillLevel = params.h2hSkillLevel or 0,
        })
        if params.kick and params.hasFootwork then
            main = params.kickDmg or 0
        end

        main = main + h2h
        off  = main
    end

    return main, off
end

-- Helper function to get Main damage depending on weapon type
xi.weaponskills.getMeleeDmg = function(attacker, weaponType, kick)
    local main, off = xi.weaponskills.meleeDmgFromParams({
        weaponType    = weaponType,
        kick          = kick,
        mainhandDmg   = attacker:getWeaponDmg(),
        offhandDmg    = attacker:getOffhandDmg(),
        h2hSkillLevel = attacker:getSkillLevel(xi.skill.HAND_TO_HAND),
        hasFootwork   = attacker:hasStatusEffect(xi.effect.FOOTWORK),
        kickDmg       = attacker:getMod(xi.mod.KICK_DMG),
    })
    return { main, off }
end

---@param attacker CBaseEntity
---@param target CBaseEntity
---@param bonus number
---@param slot xi.attackAnimation
---@return number
xi.weaponskills.getHitRate = function(attacker, target, bonus, slot)
    return xi.combat.physicalHitRate.getPhysicalHitRate(attacker, target, bonus, slot, true)
end

-- TODO: Use a common function with optional multiplier on return, or multiply outside of this.
xi.weaponskills.fTP = function(tp, ftpTable)
    if
        not ftpTable or
        tp < 1000
    then
        -- No multiplier if points are not provided, or TP is not at minimum required
        return 1
    end

    if tp >= 2000 then
        return ftpTable[2] + (tp - 2000) * (ftpTable[3] - ftpTable[2]) / 1000
    elseif tp >= 1000 then
        return ftpTable[1] + (tp - 1000) * (ftpTable[2] - ftpTable[1]) / 1000
    end
end

xi.weaponskills.calculatedIgnoredDef = function(tp, def, ignoredDefenseTable)
    if ignoredDefenseTable then
        return xi.weaponskills.fTP(tp, ignoredDefenseTable) * def
    end

    return 0
end

-----------------------------------
-- Pure: handleWeaponskillEffect apply gate (slice 6755 / internal/wsformula.ShouldApplyWeaponskillEffect)
-- params: damage, hasStatusEffect, isTargetImmune, isTargetResistant, isEffectNullified
-- returns: shouldApply (bool)
-----------------------------------
xi.weaponskills.shouldApplyWeaponskillEffectFromParams = function(params)
    params = params or {}
    return (params.damage or 0) > 0 and
        not params.hasStatusEffect and
        not params.isTargetImmune and
        not params.isTargetResistant and
        not params.isEffectNullified
end

xi.weaponskills.handleWeaponskillEffect = function(actor, target, effectId, actionElement, damage, power, duration)
    local shouldApply = xi.weaponskills.shouldApplyWeaponskillEffectFromParams({
        damage            = damage,
        hasStatusEffect   = target:hasStatusEffect(effectId),
        isTargetImmune    = xi.data.statusEffect.isTargetImmune(target, effectId, actionElement),
        isTargetResistant = xi.data.statusEffect.isTargetResistant(actor, target, effectId),
        isEffectNullified = xi.data.statusEffect.isEffectNullified(target, effectId, 0),
    })
    if shouldApply then
        target:addStatusEffect(effectId, { power = power, duration = duration, origin = actor })
    end
end
