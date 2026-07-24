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

local function shadowAbsorb(target)
    local targetShadows = target:getMod(xi.mod.UTSUSEMI)
    local shadowType    = xi.mod.UTSUSEMI

    if targetShadows == 0 then
        if math.random(1, 100) <= 80 then
            targetShadows = target:getMod(xi.mod.BLINK)
            shadowType    = xi.mod.BLINK
        end
    end

    if targetShadows > 0 then
        targetShadows = targetShadows - 1

        if shadowType == xi.mod.UTSUSEMI then
            local effect = target:getStatusEffect(xi.effect.COPY_IMAGE)
            if effect then
                if targetShadows == 1 then
                    effect:setIcon(xi.effect.COPY_IMAGE)
                elseif targetShadows == 2 then
                    effect:setIcon(xi.effect.COPY_IMAGE_2)
                elseif targetShadows == 3 then
                    effect:setIcon(xi.effect.COPY_IMAGE_3)
                end
            end
        end

        target:setMod(shadowType, targetShadows)
        if targetShadows == 0 then
            target:delStatusEffect(xi.effect.COPY_IMAGE)
            target:delStatusEffect(xi.effect.BLINK)
        end

        return true
    end

    return false
end

local function getMultiAttacks(attacker, target, wsParams, firstHit, offHand)
    local numHits      = 0
    local bonusHits    = 0
    local doubleRate   = attacker:getMod(xi.mod.DOUBLE_ATTACK) + attacker:getMerit(xi.merit.DOUBLE_ATTACK_RATE)
    local tripleRate   = attacker:getMod(xi.mod.TRIPLE_ATTACK) + attacker:getMerit(xi.merit.TRIPLE_ATTACK_RATE)
    local quadRate     = attacker:getMod(xi.mod.QUAD_ATTACK)
    local oaThriceRate = attacker:getMod(xi.mod.MYTHIC_OCC_ATT_THRICE)
    local oaTwiceRate  = attacker:getMod(xi.mod.MYTHIC_OCC_ATT_TWICE)
    local isJump       = wsParams.isJump or false

    if isJump then
        doubleRate = doubleRate + attacker:getMod(xi.mod.JUMP_DOUBLE_ATTACK)
    end

    -- TODO: Assasin vest +2 Ambush augment.
    -- The logic here wasnt actually checking for the augment.
    -- Also, it was in a completely different scale, making triple attack trigger always.

    if math.random(1, 100) <= quadRate then
        bonusHits = bonusHits + 3
    elseif math.random(1, 100) <= tripleRate then
        bonusHits = bonusHits + 2
    elseif math.random(1, 100) <= doubleRate then
        bonusHits = bonusHits + 1
    elseif firstHit and math.random(1, 100) <= oaThriceRate then -- Can only proc on first hit
        bonusHits = bonusHits + 2
    elseif firstHit and math.random(1, 100) <= oaTwiceRate then  -- Can only proc on first hit
        bonusHits = bonusHits + 1
    end

    attacker:delStatusEffect(xi.effect.ASSASSINS_CHARGE)
    attacker:delStatusEffect(xi.effect.WARRIORS_CHARGE)

    -- Try OaX for Jumps
    -- ... What's the correct dual wield interaction?
    if isJump and bonusHits == 0 and attacker:isPC() then
        -- getWeaponHitCount will always return 1 if there's a weapon in the slot, which is already accounted for.
        if offHand then
            bonusHits = attacker:getWeaponHitCount(true) - 1
        else
            bonusHits = attacker:getWeaponHitCount(false) - 1
        end
    end

    numHits = numHits + bonusHits

    return numHits
end

---@param attacker CBaseEntity
---@param target CBaseEntity
---@param bonus number
---@return number
xi.weaponskills.getRangedHitRate = function(attacker, target, bonus)
    return xi.combat.physicalHitRate.getRangedHitRate(attacker, target, bonus, true)
end

-- Function to calculate if a hit in a WS misses, criticals, and the respective damage done
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

local function getSingleHitDamage(attacker, target, dmg, ftp, wsParams, calcParams)
    local criticalHit          = false
    local hitDamage            = 0
    local atkMultiplier        = xi.weaponskills.fTP(calcParams.tpUsed, wsParams.atkVaries)
    local ignoreDefMultiplier  = xi.weaponskills.fTP(calcParams.tpUsed, wsParams.ignoredDefense)
    local applyLevelCorrection = xi.data.levelCorrection.isLevelCorrectedZone(attacker)
    local ignoresDefense       = (wsParams.ignoredDefense ~= nil) -- if the table exists, it ignores defense

    -- local pdif = 0 Reminder for Future Implementation!

    -- priority order of checks
    -- evade > parry > shadow/blink > guard/block

    -- check evasion
    local missChance = math.random()
    if xi.weaponskills.singleHitMisses(missChance, calcParams.hitRate, calcParams.guaranteedHit, calcParams.mustMiss) then
        -- miss logic
        return hitDamage, calcParams
    end

    -- check parry
    if
        xi.weaponskills.singleHitMayParry(calcParams.attackType == xi.attackType.PHYSICAL, calcParams.guaranteedHit) and
        xi.combat.physical.isParried(target, attacker)
    then
        -- parried logic
        return hitDamage, calcParams
    end

    -- check shadows
    if
        xi.weaponskills.singleHitMayShadowAbsorb(calcParams.guaranteedHit, wsParams.ignoreShadows) and
        shadowAbsorb(target)
    then
        -- shadow absorb logic
        calcParams.shadowsAbsorbed = calcParams.shadowsAbsorbed + 1
        return hitDamage, calcParams
    end

    local critChance = math.random() -- See if we land a critical hit
    -- critVaries is a truthy table when present (same as `wsParams.critVaries and ...`).
    criticalHit = xi.weaponskills.singleHitIsCritical(
        not not wsParams.critVaries,
        critChance,
        calcParams.critRate,
        calcParams.forcedFirstCrit,
        calcParams.mightyStrikesApplicable
    )

    if criticalHit then
        calcParams.criticalHit = true
    end

    if calcParams.attackType == xi.attackType.PHYSICAL then
        calcParams.pdif = xi.combat.physical.calculateMeleePDIF(attacker, target, calcParams.attackInfo.weaponType, atkMultiplier, criticalHit, applyLevelCorrection, ignoresDefense, ignoreDefMultiplier, true, calcParams.attackInfo.slot, false)
    else
        calcParams.pdif = xi.combat.physical.calculateRangedPDIF(attacker, target, calcParams.skillType, atkMultiplier, criticalHit, applyLevelCorrection, ignoresDefense, ignoreDefMultiplier, true, 0)
    end

    local blocked = xi.combat.physical.isBlocked(target, attacker)
    local blockReduction = 0
    if blocked then
        -- Reduction is computed from the pre-block product (same as upstream
        -- getDamageReductionForBlock(target, attacker, hitDamage) after the product).
        blockReduction = xi.combat.physical.getDamageReductionForBlock(
            target,
            attacker,
            (dmg + xi.combat.damage.consumeManaAddition(attacker)) * ftp * calcParams.pdif
        )
    end

    hitDamage = xi.weaponskills.singleHitDamage(
        dmg,
        xi.combat.damage.consumeManaAddition(attacker),
        ftp,
        calcParams.pdif,
        blocked,
        blockReduction
    )

    -- handle guard and reduce the hit damage if needed
    if
        calcParams.attackType == xi.attackType.PHYSICAL and
        xi.combat.physical.isGuarded(target, attacker)
    then
        calcParams.pdif        = xi.weaponskills.guardedPDIF(calcParams.pdif)
        calcParams.guardedHits = calcParams.guardedHits + 1
    end

    calcParams.hitsLanded = calcParams.hitsLanded + 1

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

local function calculateHybridMagicDamage(tp, physicaldmg, attacker, target, wsParams, calcParams, wsID)
    local ftp = xi.weaponskills.fTP(tp, wsParams.ftpMod)
    local magicdmg = xi.weaponskills.hybridWeaponskillMagicBase(
        physicaldmg,
        ftp,
        attacker:getMod(xi.mod.MAGIC_DAMAGE),
        attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
        attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID)
    )

    magicdmg = math.floor(addBonusesAbility(attacker, wsParams.ele, target, magicdmg, wsParams))
    magicdmg = xi.weaponskills.hybridWeaponskillMagicBonusFTP(magicdmg, physicaldmg, calcParams.bonusfTP or 0)

    local resist    = xi.combat.magicHitRate.calculateResistRate(attacker, target, 0, wsParams.skill, 0, wsParams.ele, 0, 0, calcParams.bonusAcc)
    local damageAdj = xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false)
    magicdmg        = xi.weaponskills.magicMitigationFloors(magicdmg, resist, damageAdj)
    magicdmg        = math.floor(target:handleSevereDamage(magicdmg, false))

    if magicdmg > 0 then
        magicdmg = xi.weaponskills.hybridMagicAbsorbNullify(
            magicdmg,
            xi.spells.damage.calculateAbsorption(target, wsParams.ele, true),
            xi.spells.damage.calculateNullification(target, wsParams.ele, true, false)
        )
    end

    if magicdmg > 0 then -- handle nonzero damage if previous function does not absorb or nullify
        magicdmg = utils.handlePhalanx(target, magicdmg)
        magicdmg = utils.handleOneForAll(target, magicdmg)
        magicdmg = utils.handleStoneskin(target, magicdmg)
    end

    return math.floor(magicdmg)
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

-- Pure magic-WS absorb/nullify when damage is non-negative (no intermediate floors).
xi.weaponskills.magicWeaponskillAbsorbNullify = function(dmg, absorb, nullify)
    return dmg * absorb * nullify
end

-- TODO: Reduce complexity
-- Disable cyclomatic complexity check for this function:
-- luacheck: ignore 561
xi.weaponskills.calculateRawWSDmg = function(attacker, target, wsID, tp, action, wsParams, calcParams)
    local targetLvl = target:getMainLvl()
    local targetHp  = xi.weaponskills.weaponskillTargetHp(target:getHP(), target:getMod(xi.mod.STONESKIN))

    -- Obtains alpha, used for working out WSC on legacy servers. Retail has no alpha anymore as of 2014 Weaponskill functions
    local alpha = 1
    if not xi.settings.main.USE_ADOULIN_WEAPON_SKILL_CHANGES then
        local level = attacker:getMainLvl()
        if level > 75 then
            alpha = 0.85
        elseif level > 59 then
            alpha = 0.9 - math.floor((level - 60) / 2) / 100
        elseif level > 5 then
            alpha = 1 - math.floor(level / 6) / 100
        end
    end

    local wsc      = xi.combat.physical.calculateWSC(attacker, wsParams.str_wsc, wsParams.dex_wsc, wsParams.vit_wsc, wsParams.agi_wsc, wsParams.int_wsc, wsParams.mnd_wsc, wsParams.chr_wsc)
    local mainBase = math.floor(calcParams.weaponDamage[1] + calcParams.fSTR + calcParams.bonusWSmods + wsc * alpha)

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
    if isRanged and attacker:isPC() then
        local ammoItem = attacker:getEquippedItem(xi.slot.AMMO)
        if ammoItem then
            ammoCount = ammoItem:getQuantity()
        else
            ammoCount = 0
        end
    end

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

    if calcParams.skillType and hitdmg > 0 then
        attacker:trySkillUp(calcParams.skillType, targetLvl)

        if isJump then
            attacker:addTP(mainhandTPGain * attackerTPMult)
        end

        calcParams.tpHitsLanded   = 1 -- Store number of TP hits that have landed thus far
        calcParams.mainHitsLanded = 0
    end

    finaldmg = finaldmg + hitdmg

    -- Finish first/mainhand hit

    local numMainHandMultis = xi.weaponskills.initialMainhandMultis(isRanged, isRanged and 0 or getMultiAttacks(attacker, target, wsParams, true, false))
    local numOffhandMultis  = 0
    local numMultiProcs     = xi.weaponskills.initialMultiProcCount(numMainHandMultis)

    -- Have to calculate added bonus for SA/TA here since it is done outside of the fTP multiplier
    if attacker:getMainJob() == xi.job.THF then
        -- Add DEX/AGI bonus to base damage of first hit if THF main and valid Sneak/Trick Attack
        if calcParams.sneakApplicable then
            finaldmg = xi.weaponskills.weaponskillSneakAttackBonus(
                finaldmg,
                calcParams.pdif,
                attacker:getStat(xi.mod.DEX),
                attacker:getMod(xi.mod.SNEAK_ATK_DEX)
            )
        end

        if calcParams.trickApplicable then
            finaldmg = xi.weaponskills.weaponskillTrickAttackBonus(
                finaldmg,
                calcParams.pdif,
                attacker:getStat(xi.mod.AGI),
                attacker:getMod(xi.mod.TRICK_ATK_AGI)
            )
        end
    end

    -- these are deliberately left outside of the "If main job is THF" if-statement
    if calcParams.sneakApplicable then
        finaldmg = xi.weaponskills.weaponskillAugmentSA(finaldmg, attacker:getMod(xi.mod.AUGMENTS_SA))
    end

    if calcParams.trickApplicable then
        finaldmg = xi.weaponskills.weaponskillAugmentTA(finaldmg, attacker:getMod(xi.mod.AUGMENTS_TA))
    end

    -- We've now accounted for any crit from SA/TA, so nullify them
    calcParams.forcedFirstCrit = false

    -- For items that apply bonus damage to the first hit of a weaponskill (but not later hits),
    -- store bonus damage for first hit, for use after other calculations are done
    local firstHitBonus = xi.weaponskills.weaponskillFirstHitBonus(finaldmg, attacker:getMod(xi.mod.ALL_WSDMG_FIRST_HIT))

    -- Reset fTP if it's not supposed to carry over across all hits for this WS
    -- We'll recalculate our mainhand damage after doing offhand
    ftp = xi.weaponskills.multiHitFTPAfterFirst(ftp, wsParams.multiHitfTP)

    local offhandSkill = attacker:getWeaponSkillType(xi.slot.SUB)
    local isH2H        = false
    if calcParams.skillType == xi.skill.HAND_TO_HAND then
        offhandSkill = xi.skill.HAND_TO_HAND
        subTPGain    = mainhandTPGain
        isH2H        = true
    end

    calcParams.guaranteedHit = false -- Accuracy bonus from SA/TA applies only to first main and offhand hit

    dmg = mainBase

    -- First mainhand hit is already accounted for
    local mainhandHits     = xi.weaponskills.mainhandHitsRemaining(wsParams.numHits)
    local mainhandHitsDone = 0

    if isRanged and ammoCount ~= -1 then
        ammoUsed = ammoUsed + useAmmo(attacker)

        if xi.weaponskills.ammoExhausted(ammoUsed, ammoCount) then
            hitsDone = 8 -- Attack while loops will stop if hitsDone is 8 or higher
        end
    end

    -- Use up any remaining hits in the WS's numhits
    while mainhandHitsDone < mainhandHits and xi.weaponskills.wsHitLoopContinues(hitsDone, finaldmg, targetHp) do
        hitdmg, calcParams    = getSingleHitDamage(attacker, target, dmg, ftp, wsParams, calcParams)

        if calcParams.melee then
            hitdmg = modifyMeleeHitDamage(attacker, target, calcParams.attackInfo, wsParams, hitdmg)
        end

        if hitdmg > 0 then
            attacker:trySkillUp(calcParams.skillType, targetLvl)

            -- When dual wielding, the mainhand appears to count the second hit as a TP hit unless it's a 1 hit WS where the offhand will gain TP
            -- H2H also does this on retail (much more easy to verify)
            -- Needs better verification
            local tpClass = xi.weaponskills.classifyLandedHitTP(isH2H, calcParams.extraOffhandHit, hitsDone, wsParams.isBarrage, false)
            if tpClass == 'tp' then
                calcParams.tpHitsLanded = calcParams.tpHitsLanded + 1
            else
                calcParams.mainHitsLanded = calcParams.mainHitsLanded + 1
            end

            if isJump then
                attacker:addTP(mainhandTPGain * attackerTPMult)
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

        if isRanged and ammoCount ~= -1 then
            ammoUsed = ammoUsed + useAmmo(attacker)

            if xi.weaponskills.ammoExhausted(ammoUsed, ammoCount) then
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

        if hitdmg > 0 then
            attacker:trySkillUp(calcParams.skillType, targetLvl)

            -- When dual wielding, the mainhand appears to count the second hit as a TP hit unless it's a 1 hit WS where the offhand will gain TP
            -- Needs better verification, in this case (1 hit ws with multis)  a DA/TA/QA may not count as TP hit and we'd move this into the offhand hit proc.
            -- Either way, this won't "cheat" players out of TP in the current implementation.
            local tpClass = xi.weaponskills.classifyLandedHitTP(false, calcParams.extraOffhandHit, hitsDone, false, false)
            if tpClass == 'tp' then
                calcParams.tpHitsLanded = calcParams.tpHitsLanded + 1
            else
                calcParams.mainHitsLanded = calcParams.mainHitsLanded + 1
            end

            if isJump then
                attacker:addTP(mainhandTPGain * attackerTPMult)
            end
        end

        finaldmg                  = finaldmg + hitdmg
        hitsDone                  = hitsDone + 1
        mainhandMultiHitsDone     = mainhandMultiHitsDone + 1

        if isRanged and ammoCount ~= -1 then
            ammoUsed = ammoUsed + useAmmo(attacker)

            if xi.weaponskills.ammoExhausted(ammoUsed, ammoCount) then
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

        if hitdmg > 0 then
            attacker:trySkillUp(offhandSkill, targetLvl)

            -- If this is the second swing of the WS (1 hit ws) the offhand appears to count for TP gain
            -- Needs better verification
            local tpClass = xi.weaponskills.classifyLandedHitTP(false, true, hitsDone, false, true)
            if tpClass == 'tp' then
                calcParams.tpHitsLanded = calcParams.tpHitsLanded + 1
            else
                calcParams.offhandHitsLanded = calcParams.offhandHitsLanded + 1
            end

            if isJump then
                attacker:addTP(subTPGain * attackerTPMult)
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

        if hitdmg > 0 then
            attacker:trySkillUp(offhandSkill, targetLvl)

            if isJump then
                attacker:addTP(subTPGain * attackerTPMult)
            end

            calcParams.offhandHitsLanded = calcParams.offhandHitsLanded + 1
        end

        finaldmg             = finaldmg + hitdmg
        hitsDone             = hitsDone + 1
        offhandMultiHitsDone = offhandMultiHitsDone + 1
    end

    calcParams.extraHitsLanded = calcParams.mainHitsLanded + calcParams.offhandHitsLanded

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
    calcParams.mustMiss                = xi.weaponskills.physicalMustMiss(
        target:hasStatusEffect(xi.effect.PERFECT_DODGE),
        target:hasStatusEffect(xi.effect.ALL_MISS),
        wsParams.hitsHigh
    )
    calcParams.sneakApplicable         = xi.weaponskills.sneakApplicable(
        attacker:hasStatusEffect(xi.effect.SNEAK_ATTACK),
        attacker:isBehind(target),
        attacker:hasStatusEffect(xi.effect.HIDE),
        target:hasStatusEffect(xi.effect.DOUBT)
    )
    calcParams.taChar                  = taChar
    calcParams.trickApplicable         = xi.weaponskills.trickApplicable(calcParams.taChar ~= nil)
    calcParams.assassinApplicable      = xi.weaponskills.assassinApplicable(calcParams.trickApplicable, attacker:hasTrait(xi.trait.ASSASSIN))
    calcParams.guaranteedHit           = xi.weaponskills.guaranteedHit(calcParams.sneakApplicable, calcParams.trickApplicable)
    calcParams.mightyStrikesApplicable = attacker:hasStatusEffect(xi.effect.MIGHTY_STRIKES)
    calcParams.forcedFirstCrit         = xi.weaponskills.forcedFirstCrit(calcParams.sneakApplicable, calcParams.assassinApplicable)
    calcParams.extraOffhandHit         = attacker:isDualWielding()
    calcParams.hybridHit               = wsParams.hybridWS
    calcParams.flourishEffect          = attacker:getStatusEffect(xi.effect.BUILDING_FLOURISH)
    calcParams.bonusTP                 = wsParams.bonusTP or 0
    calcParams.tpUsed                  = tp
    calcParams.attackType              = xi.attackType.PHYSICAL

    local isJump = wsParams.isJump or false
    calcParams.bonusfTP, calcParams.bonusAcc, calcParams.bonusWSmods = xi.weaponskills.physicalBonusInjects(
        isJump,
        gearFTP,
        gearAcc,
        attacker:getMod(xi.mod.JUMP_ACC_BONUS),
        attacker:getMod(xi.mod.WSACC),
        wsParams.bonusWSmods
    )

    if wsParams.accVaries then
        calcParams.bonusAcc = xi.weaponskills.bonusAccWithVaries(calcParams.bonusAcc, xi.weaponskills.fTP(tp, wsParams.accVaries))
    end

    calcParams.firstHitRate = xi.weaponskills.getHitRate(attacker, target, xi.weaponskills.firstHitAccuracyBonus(calcParams.bonusAcc), xi.attackAnimation.RIGHT_ATTACK)
    calcParams.hitRate      = xi.weaponskills.getHitRate(attacker, target, calcParams.bonusAcc, xi.attackAnimation.RIGHT_ATTACK)
    calcParams.skillType    = attack.weaponType

    -- Send our wsParams off to calculate our raw WS damage, hits landed, and shadows absorbed
    calcParams     = xi.weaponskills.calculateRawWSDmg(attacker, target, wsID, tp, action, wsParams, calcParams)
    local finaldmg = math.floor(calcParams.finalDmg)

    -- Add in magic damage for hybrid weaponskills
    -- Only procs if the mob still has HP remaining
    if xi.weaponskills.hybridMagicApplies(wsParams.hybridWS, target:getHP(), finaldmg) then
        finaldmg = finaldmg + calculateHybridMagicDamage(tp, finaldmg, attacker, target, wsParams, calcParams, wsID)
    end

    -- Delete statuses that may have been spent by the WS
    attacker:delStatusEffectsByFlag(xi.effectFlag.DETECTABLE)
    attacker:delStatusEffect(xi.effect.SNEAK_ATTACK)
    attacker:delStatusEffectSilent(xi.effect.BUILDING_FLOURISH)

    finaldmg            = xi.weaponskills.applyWeaponSkillPower(finaldmg, xi.settings.main.WEAPON_SKILL_POWER) -- Add server bonus
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

    local _, bonusAcc, bonusWSmods = xi.weaponskills.physicalBonusInjects(
        false,
        gearFTP,
        gearAcc,
        0,
        attacker:getMod(xi.mod.WSACC),
        wsParams.bonusWSmods
    )

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
        bonusfTP                = gearFTP,
        bonusAcc                = bonusAcc,
        bonusWSmods             = bonusWSmods,
        attackType              = xi.attackType.RANGED,
    }

    if wsParams.accVaries then
        calcParams.bonusAcc = xi.weaponskills.bonusAccWithVaries(calcParams.bonusAcc, xi.weaponskills.fTP(tp, wsParams.accVaries))
    end

    -- Split Shot/Piercing Arrow and Empyreal Arrow/Detonator are confirmed for this. Theoretically Last Stand could have a bonus too, and if so it would likely be first hit only.
    if wsParams.rangedAccuracyBonus then
        calcParams.firstHitRate = xi.weaponskills.getRangedHitRate(attacker, target, calcParams.bonusAcc + wsParams.rangedAccuracyBonus)
    end

    calcParams.hitRate = xi.weaponskills.getRangedHitRate(attacker, target, calcParams.bonusAcc)

    -- Send our params off to calculate our raw WS damage, hits landed, and shadows absorbed
    calcParams = xi.weaponskills.calculateRawWSDmg(attacker, target, wsID, tp, action, wsParams, calcParams)
    local finaldmg = calcParams.finalDmg

    -- Delete statuses that may have been spent by the WS
    attacker:delStatusEffectsByFlag(xi.effectFlag.DETECTABLE)
    attacker:delStatusEffect(xi.effect.FLASHY_SHOT)
    attacker:delStatusEffect(xi.effect.STEALTH_SHOT)

    -- Calculate reductions
    finaldmg = xi.weaponskills.rangedWeaponskillMitigation(
        target:rangedDmgTaken(finaldmg),
        target:getMod(xi.mod.PIERCE_SDT)
    )

    -- Add in magic damage for hybrid weaponskills
    -- Only procs if the mob still has HP remaining
    if xi.weaponskills.hybridMagicApplies(wsParams.hybridWS, target:getHP(), finaldmg) then
        finaldmg = finaldmg + calculateHybridMagicDamage(tp, finaldmg, attacker, target, wsParams, calcParams, wsID)
    end

    finaldmg            = xi.weaponskills.applyWeaponSkillPower(finaldmg, xi.settings.main.WEAPON_SKILL_POWER) -- Add server bonus
    calcParams.finalDmg = finaldmg

    finaldmg = xi.weaponskills.takeWeaponskillDamage(target, attacker, wsParams, primaryMsg, attack, calcParams, action)

    -- Ammo needs to be removed after xi.weaponskills.takeWeaponskillDamage for delay/tp return uses
    if calcParams.ammoUsed and calcParams.ammoUsed > 0 then
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

xi.weaponskills.doMagicWeaponskill = function(attacker, target, wsID, wsParams, tp, action, primaryMsg)
    -- Set up conditions and wsParams used for calculating weaponskill damage
    local attack =
    {
        ['type']       = xi.attackType.MAGICAL,
        ['slot']       = xi.slot.MAIN,
        ['weaponType'] = attacker:getWeaponSkillType(xi.slot.MAIN),
        ['damageType'] = xi.damageType.ELEMENTAL + wsParams.ele
    }

    local calcParams =
    {
        ['shadowsAbsorbed'] = 0,
        ['hitsLanded']      = 1,
        ['tpHitsLanded']    = 1,
        ['extraHitsLanded'] = 0,
        ['bonusTP']         = wsParams.bonusTP or 0,
        ['wsID']            = wsID,
    }

    if xi.weaponskills.isRangedMagicWeaponskill(wsParams.skill) then
        attack.slot = xi.slot.RANGED
    end

    local dStat   = wsParams.dStat and wsParams.dStat or xi.mod.INT
    local gearFTP = xi.combat.physical.calculateFTPBonus(attacker)
    local gearAcc = xi.weaponskills.gearAccFromFTP(gearFTP) + attacker:getMod(xi.mod.WSACC) -- TODO: Separate gear fTP and acc bonuses
    local fint    = 0
    local dmg     = 0

    -- TODO: ranged magic WS are universal in it's (AGI-INT)*2
    -- But other magic WS vary. Some don't even have a component, the general case is dINT/2 + 8
    fint = xi.weaponskills.magicWeaponskillFint(attack.slot == xi.slot.RANGED, dStat == xi.mod.CHR, attacker:getStat(dStat) - target:getStat(xi.mod.INT))

    -- Magic-based WSes never miss, so we don't need to worry about calculating a miss, only if a shadow absorbed it.
    if not shadowAbsorb(target) then
        local wsc = xi.combat.physical.calculateWSC(attacker, wsParams.str_wsc, wsParams.dex_wsc, wsParams.vit_wsc, wsParams.agi_wsc, wsParams.int_wsc, wsParams.mnd_wsc, wsParams.chr_wsc)
        local ftp = xi.weaponskills.fTP(tp, wsParams.ftpMod)

        dmg = xi.weaponskills.magicWeaponskillRawDamage(wsc, attacker:getMainLvl(), fint, ftp, gearFTP)

        -- Apply Consume Mana and Scarlet Delirium
        -- dmg = dmg + xi.combat.damage.consumeManaAddition(attacker)
        dmg = dmg * xi.combat.damage.scarletDeliriumMultiplier(attacker)

        dmg = xi.weaponskills.magicWeaponskillWSDProduct(
            dmg,
            attacker:getMod(xi.mod.ALL_WSDMG_ALL_HITS),
            attacker:getMod(xi.mod.WEAPONSKILL_DAMAGE_BASE + wsID),
            attacker:isPet(),
            attacker:getMod(xi.mod.ALL_WSDMG_FIRST_HIT)
        )

        -- Calculate magical bonuses and reductions
        dmg = math.floor(addBonusesAbility(attacker, wsParams.ele, target, dmg, wsParams))
        dmg = xi.weaponskills.magicMitigationFloors(
            dmg,
            xi.combat.magicHitRate.calculateResistRate(attacker, target, 0, wsParams.skill, 0, wsParams.ele, 0, 0, gearAcc),
            xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false)
        )
        dmg = math.floor(target:handleSevereDamage(dmg, false))

        if dmg < 0 then
            calcParams.finalDmg = dmg

            dmg = xi.weaponskills.takeWeaponskillDamage(target, attacker, wsParams, primaryMsg, attack, calcParams, action)
            return dmg
        end

        dmg = xi.weaponskills.magicWeaponskillAbsorbNullify(
            dmg,
            xi.spells.damage.calculateAbsorption(target, wsParams.ele, true),
            xi.spells.damage.calculateNullification(target, wsParams.ele, true, false)
        )

        dmg = utils.handlePhalanx(target, dmg)
        dmg = utils.handleOneForAll(target, dmg)
        dmg = utils.handleStoneskin(target, dmg)

        dmg = dmg * xi.settings.main.WEAPON_SKILL_POWER -- Add server bonus
    else
        calcParams.shadowsAbsorbed = 1
    end

    calcParams.finalDmg = dmg

    if dmg > 0 then
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

    local targetTPMult   = wsParams.targetTPMult or 1
    local attackerTPMult = wsParams.attackerTPMult or 1
    local isJump         = wsParams.isJump or false

    -- DA/TA/QA/OaT/Oa2-3 etc give full TP return per hit on Jumps
    attackerTPMult, wsResults.extraHitsLanded = xi.weaponskills.jumpTPAdjust(isJump, attackerTPMult, wsResults.extraHitsLanded)

    -- Core does not modify the TP for the 10 TP/hit like it should, so we're doing it here
    local storeTPModifier = xi.weaponskills.storeTPModifier(attacker:getMod(xi.mod.STORETP)) -- TODO, make a global function to get this (inhibit TP is not accounted for properly in core)
    local extraHitTP     = xi.weaponskills.weaponskillExtraHitTP(wsResults.extraHitsLanded, storeTPModifier, wsResults.bonusTP)

    finaldmg = defender:takeWeaponskillDamage(attacker, finaldmg, attack.type, attack.damageType, attack.slot, primaryMsg, wsResults.tpHitsLanded * attackerTPMult, extraHitTP, targetTPMult)
    if wsResults.tpHitsLanded + wsResults.extraHitsLanded > 0 then
        action:recordDamage(defender, attack.type, math.abs(finaldmg), wsResults.criticalHit)
    end

    local enmityEntity = wsResults.taChar or attacker

    if xi.weaponskills.weaponskillEnmityUsesOverride(wsParams.overrideCE, wsParams.overrideVE, wsResults.tpHitsLanded, wsResults.extraHitsLanded) then
        defender:addEnmity(enmityEntity, wsParams.overrideCE, wsParams.overrideVE)
    else
        local enmityMult = wsParams.enmityMult or 1
        defender:updateEnmityFromDamage(enmityEntity, finaldmg * enmityMult)
    end

    local sengikoriEffect = attacker:getStatusEffect(xi.effect.SENGIKORI)
    -- TODO: does this effect not apply if you do 0 damage (or absorb)?
    -- Skillchains will still "proc" if you do 0 damage, so assume it does for now
    if
        (wsResults.tpHitsLanded +
        wsResults.extraHitsLanded > 0) and
        sengikoriEffect ~= nil
    then
        local sengikoriBonus = attacker:getMod(xi.mod.SENGIKORI_BONUS) -- Additive % bonus: https://www.ffxiah.com/forum/topic/23457/july-11-sam-update/4/#1421344
        local power = sengikoriEffect:getPower() + sengikoriBonus

        -- If no SC effect, apply SC damage debuff
        -- If there is one, apply MB damage debuff
        -- This "effect" lasts until the it's used or the SC goes away
        -- see https://wiki.ffo.jp/html/20051.html
        if defender:hasStatusEffect(xi.effect.SKILLCHAIN) then
            defender:setMod(xi.mod.SENGIKORI_MB_DMG_DEBUFF, power)
        else
            defender:setMod(xi.mod.SENGIKORI_SC_DMG_DEBUFF, power)
        end

        attacker:delStatusEffect(xi.effect.SENGIKORI)
    end

    if finaldmg > 0 then
        -- Pack the weaponskill ID in the top 8 bits of this variable which is utilized
        -- in OnMobDeath in luautils.  Max WSID is 255.
        defender:setLocalVar('weaponskillHit', bit.lshift(wsResults.wsID, 24) + finaldmg)
    end

    return finaldmg
end

-- Helper function to get Main damage depending on weapon type
xi.weaponskills.getMeleeDmg = function(attacker, weaponType, kick)
    local mainhandDamage = attacker:getWeaponDmg()
    local offhandDamage  = attacker:getOffhandDmg()

    if weaponType == xi.skill.HAND_TO_HAND or weaponType == xi.skill.NONE then
        local h2hSkill = attacker:getSkillLevel(xi.skill.HAND_TO_HAND) * 0.11 + 3

        if kick and attacker:hasStatusEffect(xi.effect.FOOTWORK) then
            mainhandDamage = attacker:getMod(xi.mod.KICK_DMG) -- Use Kick damage if footwork is on
        end

        mainhandDamage = mainhandDamage + h2hSkill
        offhandDamage  = mainhandDamage
    end

    return { mainhandDamage, offhandDamage }
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

xi.weaponskills.handleWeaponskillEffect = function(actor, target, effectId, actionElement, damage, power, duration)
    if
        damage > 0 and
        not target:hasStatusEffect(effectId) and
        not xi.data.statusEffect.isTargetImmune(target, effectId, actionElement) and
        not xi.data.statusEffect.isTargetResistant(actor, target, effectId) and
        not xi.data.statusEffect.isEffectNullified(target, effectId, 0)
    then
        target:addStatusEffect(effectId, { power = power, duration = duration, origin = actor })
    end
end
