xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.tp = xi.combat.tp or {}
-----------------------------------

-----------------------------------
-- Pure formula helpers (OmegaXI slice 6679)
-- Dual-wired so Go internal/attackutils and LSB tests share one surface.
-----------------------------------

-- Pure classification: charmed TYPE_MOB under a PC master uses the PC table.
xi.combat.tp.isCharmedPCPet = function(isMob, isCharmed, hasPCMaster)
    return isMob and isCharmed and hasPCMaster
end

-- Pure classification: PC/pet/trust (or charmed PC pet) use the PC delay→TP table.
xi.combat.tp.usePCOrPetTPFormula = function(isMob, isCharmedPCPet)
    return (not isMob) or isCharmedPCPet
end

-- Pure delay→TP tables once the formula flag is known (math.floor toward -inf).
-- isPCOrPetFormula true → PC/pet bands; false → mob bands.
-- USED IN CORE via calculateTPReturn host (params must stay mirrored in core).
-- https://www.bg-wiki.com/ffxi/Tactical_Points
-- Mob formula: http://wiki.ffo.jp/html/308.html
xi.combat.tp.calculateTPReturnFromDelay = function(isPCOrPetFormula, delay)
    local tpReturn = 0

    if isPCOrPetFormula then -- Pets and PCs have been observed to use this formula
        if delay > 900 then
            tpReturn = 173 + (delay - 900) * 28 / 360
        elseif delay > 720 then
            tpReturn = 161 + (delay - 720) * 24 / 360
        elseif delay > 630 then
            tpReturn = 154 + (delay - 630) * 28 / 360
        elseif delay > 540 then
            tpReturn = 149 + (delay - 540) * 20 / 360
        elseif delay > 180 then
            tpReturn = 61 + (delay - 180) * 88 / 360
        else
            tpReturn = 61 + (delay - 180) * 63 / 360
        end
    else -- mobs have been observed to use this formula
        if delay > 530 then
            tpReturn = 145 + (delay - 530) * 35 / 470
        elseif delay > 480 then
            tpReturn = 130 + (delay - 480) * 15 / 30
        elseif delay > 450 then
            tpReturn = 115 + (delay - 450) * 15 / 30
        elseif delay > 180 then
            tpReturn = 50 + (delay - 180) * 65 / 270
        else
            tpReturn = 50 + (delay - 180) * 15 / 180
        end
    end

    return math.floor(tpReturn)
end

-----------------------------------
-- Entity hosts (entity injects → pure)
-----------------------------------

-- USED IN CORE (If you add/remove function params, they must be mirrored in core)
-- Gainee is the target who is going to gain the TP.
-- For instance, if a player attacks a mob, the mob uses the mob formula when gaining TP from the returned hit.
-- This appears to be a measure to not buff mobs when players were buffed with the new TP gain formula.
--- @params gainee CBaseEntity
--- @params delay integer
--- @return integer
xi.combat.tp.calculateTPReturn = function(gainee, delay)
    local isMob = false
    local isCharmed = false
    local hasPCMaster = false

    if gainee then
        isMob = gainee:getObjType() == xi.objType.MOB
        if isMob then
            -- Charmed pets controlled by the player are not caught by isPet() and are
            -- considered mobs still. Once charmed, they convert to the PC delay formula.
            isCharmed = gainee:isCharmed()
            local master = gainee:getMaster()
            hasPCMaster = master ~= nil and master:isPC()
        end
    else
        -- nil gainee: original (gainee and ...) is falsy → mob formula unless charmed
        -- (which cannot apply without an entity). Force mob table.
        isMob = true
    end

    local isCharmedPCPet = xi.combat.tp.isCharmedPCPet(isMob, isCharmed, hasPCMaster)
    local isPCOrPetFormula = xi.combat.tp.usePCOrPetTPFormula(isMob, isCharmedPCPet)

    return xi.combat.tp.calculateTPReturnFromDelay(isPCOrPetFormula, delay)
end

-----------------------------------
-- Pure modified-delay / Zanshin helpers (OmegaXI slice 6680)
-- Dual-wired to internal/attackutils.GetModifiedDelayAndCanZanshin.
-----------------------------------

-- Actor kind for H2H branch (PC / MOB / residual pet-trust-etc).
xi.combat.tp.modifiedDelayActor =
{
    PC    = 0,
    MOB   = 1,
    OTHER = 2,
}

-- H2H floors and DELAYP mult floor (Attack_Speed).
xi.combat.tp.h2hSingleSwingMinDelay = 96
xi.combat.tp.h2hFistMinDelay        = 48
xi.combat.tp.delayPMinMultiplier    = 0.85

-- Pure delay rewrite once dual-wield / H2H / actor kind / mods are injected.
-- params:
--   delay, dualWield, dualWieldMod, usingH2H, actorKind (modifiedDelayActor),
--   subEquipped, h2hSkillRankZero, martialArtsMod, delayP
-- returns { canZanshin = bool, modifiedDelay = floor(...) }
-- https://www.bg-wiki.com/ffxi/Tactical_Points
-- https://www.bg-wiki.com/ffxi/Attack_Speed
-- https://www.bg-wiki.com/ffxi/Zanshin
xi.combat.tp.getModifiedDelayAndCanZanshinFromParams = function(params)
    local delay          = params.delay or 0
    local dualWield      = params.dualWield or false
    local dualWieldMod   = params.dualWieldMod or 0
    local usingH2H       = params.usingH2H or false
    local actorKind      = params.actorKind or xi.combat.tp.modifiedDelayActor.OTHER
    local subEquipped    = params.subEquipped or false
    local h2hRankZero    = params.h2hSkillRankZero or false
    local martialArts    = params.martialArtsMod or 0
    local delayP         = params.delayP or 0

    local modifiedDelay = delay
    local canZanshin    = false

    -- DW/H2H delay is halved for a single hit's TP return when applicable.
    -- NOTE: isDualWielding may trip on non-PCs even if they are using H2H.
    if dualWield then
        modifiedDelay = (delay * (100 - dualWieldMod) / 100) / 2
    elseif usingH2H then
        if actorKind == xi.combat.tp.modifiedDelayActor.PC then
            if subEquipped or h2hRankZero then
                modifiedDelay = math.max(delay - martialArts, xi.combat.tp.h2hSingleSwingMinDelay)
                canZanshin    = true -- unarmed / single-fist swing
            else
                modifiedDelay = math.max((delay - martialArts) / 2, xi.combat.tp.h2hFistMinDelay)
            end
        elseif actorKind == xi.combat.tp.modifiedDelayActor.MOB then
            -- Mobs are not affected at all by Martial Arts.
            modifiedDelay = math.max(delay / 2, xi.combat.tp.h2hFistMinDelay)
        else
            -- Pet/trust/etc. residual H2H arm.
            modifiedDelay = math.max((delay - martialArts) / 2, xi.combat.tp.h2hFistMinDelay)
        end
    else -- single melee swing, either 1H or 2H
        canZanshin = true
    end

    -- DELAYP scale with -15% floor.
    modifiedDelay = modifiedDelay * math.max((100 + delayP) / 100, xi.combat.tp.delayPMinMultiplier)

    return ({ canZanshin = canZanshin, modifiedDelay = math.floor(modifiedDelay) })
end

-- Entity host: dual-wield / H2H / equip / skill-rank / mod reads → pure.
xi.combat.tp.getModifiedDelayAndCanZanshin = function(actor, delay)
    local actorKind = xi.combat.tp.modifiedDelayActor.OTHER
    local objType   = actor:getObjType()

    if objType == xi.objType.PC then
        actorKind = xi.combat.tp.modifiedDelayActor.PC
    elseif objType == xi.objType.MOB then
        actorKind = xi.combat.tp.modifiedDelayActor.MOB
    end

    return xi.combat.tp.getModifiedDelayAndCanZanshinFromParams({
        delay            = delay,
        dualWield        = actor:isDualWielding(),
        dualWieldMod     = actor:getMod(xi.mod.DUAL_WIELD),
        usingH2H         = actor:isUsingH2H(),
        actorKind        = actorKind,
        subEquipped      = actor:getEquippedItem(xi.slot.SUB) ~= nil,
        h2hSkillRankZero = actor:getSkillRank(xi.skill.HAND_TO_HAND) == 0,
        martialArtsMod   = actor:getMod(xi.mod.MARTIAL_ARTS),
        delayP           = actor:getMod(xi.mod.DELAYP),
    })
end

-----------------------------------
-- Pure defender TP-gain helpers (OmegaXI slice 6682)
-- Dual-wired to internal/tpgain. dAGI pure keeps LSB operator-precedence
-- production form (see Go DAGIModifier intentional-curve note).
-----------------------------------

xi.combat.tp.subtleBlowICap   = 50
xi.combat.tp.subtleBlowModMin = 0.25
xi.combat.tp.dagiModMin       = 0.5
xi.combat.tp.dagiModMax       = 1.0
xi.combat.tp.magicBaseMob     = 100
xi.combat.tp.magicBaseNonMob  = 50
xi.combat.tp.physicalMobBaseBonus = 30
xi.combat.tp.physicalNonMobShare  = 1 / 3

-- Pure (100 - INHIBIT_TP) / 100. No known cap.
xi.combat.tp.inhibitTPModifier = function(inhibitTP)
    return (100 - (inhibitTP or 0)) / 100
end

-- Pure 1 + STORETP/100.
xi.combat.tp.storeTPModifier = function(storeTP)
    return 1 + (storeTP or 0) / 100
end

-- Pure Subtle Blow I: min(mod + merits, 50).
xi.combat.tp.subtleBlowI = function(subtleBlow, merits)
    return math.min((subtleBlow or 0) + (merits or 0), xi.combat.tp.subtleBlowICap)
end

-- Pure Subtle Blow II term: mod + tandem bonus.
xi.combat.tp.subtleBlowII = function(subtleBlowII, tandemBlowBonus)
    return (subtleBlowII or 0) + (tandemBlowBonus or 0)
end

-- Pure combined subtle-blow multiplier:
--   max((100 - subtleBlowI + subtleBlowII) / 100, 0.25)
xi.combat.tp.subtleBlowModifier = function(subtleBlowI, subtleBlowII)
    return math.max((100 - (subtleBlowI or 0) + (subtleBlowII or 0)) / 100, xi.combat.tp.subtleBlowModMin)
end

-- Pure dAGI TP-feed scale using production LSB arithmetic (operator precedence
-- as written: 200 - (dAGI+30)/200, then clamp 0.5..1).
-- Go internal/tpgain.DAGIModifier follows the *documented* intended curve instead.
xi.combat.tp.dagiModifier = function(dAGI)
    dAGI = dAGI or 0

    return utils.clamp(200 - (dAGI + 30) / 200, xi.combat.tp.dagiModMin, xi.combat.tp.dagiModMax)
end

-- Pure Tandem Blow power once tandem activity and power sources are known.
xi.combat.tp.tandemBlowBonus = function(tandemActive, hasMasterPC, masterPower, selfPower)
    if not tandemActive then
        return 0
    end

    if hasMasterPC then
        return masterPower or 0
    end

    return selfPower or 0
end

-- Pure physical defender TP-gain product after gates and base TP are known.
-- params: baseTPGain, targetIsMob, actorIsMob, dAGI, inhibitTP, storeTP,
--         subtleBlow, subtleBlowMerit, subtleBlowII, tandemBlowBonus
xi.combat.tp.physicalTPGain = function(params)
    local inhibit = xi.combat.tp.inhibitTPModifier(params.inhibitTP)
    local subtleI = xi.combat.tp.subtleBlowI(params.subtleBlow, params.subtleBlowMerit)
    local subtleII = xi.combat.tp.subtleBlowII(params.subtleBlowII, params.tandemBlowBonus)
    local subtle = xi.combat.tp.subtleBlowModifier(subtleI, subtleII)
    local store = xi.combat.tp.storeTPModifier(params.storeTP)
    local base = params.baseTPGain or 0

    if params.targetIsMob and not params.actorIsMob then
        local dagi = xi.combat.tp.dagiModifier(params.dAGI)

        return math.floor((base + xi.combat.tp.physicalMobBaseBonus) * inhibit * dagi * subtle * store)
    end

    return math.floor(base * inhibit * subtle * store * xi.combat.tp.physicalNonMobShare)
end

-- Pure magical defender TP-gain product after gates.
-- params: targetIsMob, dAGI, inhibitTP, storeTP, subtleBlow, subtleBlowMerit,
--         subtleBlowII, tandemBlowBonus
xi.combat.tp.magicalTPGain = function(params)
    local inhibit = xi.combat.tp.inhibitTPModifier(params.inhibitTP)
    local subtleI = xi.combat.tp.subtleBlowI(params.subtleBlow, params.subtleBlowMerit)
    local subtleII = xi.combat.tp.subtleBlowII(params.subtleBlowII, params.tandemBlowBonus)
    local subtle = xi.combat.tp.subtleBlowModifier(subtleI, subtleII)
    local store = xi.combat.tp.storeTPModifier(params.storeTP)

    if params.targetIsMob then
        local dagi = xi.combat.tp.dagiModifier(params.dAGI)

        return math.floor(xi.combat.tp.magicBaseMob * inhibit * dagi * subtle * store)
    end

    return math.floor(xi.combat.tp.magicBaseNonMob * inhibit * subtle * store)
end

-- Pure Occult Acumen spell TP once PC/Meikyo/skill/MP/mods are injected.
-- params: isPC, meikyoShisui, skillEligible, mpCost, occultAcumen, storeTP
xi.combat.tp.spellTP = function(params)
    if not params.isPC or params.meikyoShisui or not params.skillEligible then
        return 0
    end

    local occult = (params.occultAcumen or 0) / 100
    local store = xi.combat.tp.storeTPModifier(params.storeTP)

    return math.floor((params.mpCost or 0) * occult * store)
end

-- Bonus subtle blow II from Tandem Blow (BST trait) — entity host.
xi.combat.tp.getTandemBlowBonus = function(actor)
    local hasMaster = actor:getMaster() ~= nil
    local hasMasterPC = hasMaster and actor:getMaster():isPC()
    local masterPower = 0
    local selfPower = actor:getMod(xi.mod.TANDEM_BLOW_POWER)

    if hasMasterPC then
        masterPower = actor:getMaster():getMod(xi.mod.TANDEM_BLOW_POWER)
    end

    return xi.combat.tp.tandemBlowBonus(actor:isTandemActive(), hasMasterPC, masterPower, selfPower)
end

-----------------------------------
-- Global functions used elsewhere.
-----------------------------------

-- Returns attacker TP gain from a single melee hit from itself.
xi.combat.tp.getSingleMeleeHitTPReturn = function(actor, isZanshin)
    if actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI) then
        return 0
    end

    isZanshin = isZanshin or false -- optional input, defaults to false.

    local delay        = actor:getBaseDelay()
    local attackOutput = xi.combat.tp.getModifiedDelayAndCanZanshin(actor, delay)
    local tpReturn     = xi.combat.tp.calculateTPReturn(actor, attackOutput.modifiedDelay)

    if isZanshin and attackOutput.canZanshin then
        tpReturn = tpReturn + actor:getMerit(xi.merit.IKISHOTEN) -- https://www.bg-wiki.com/ffxi/Ikishoten
    end

    local storeTPModifier = xi.combat.tp.storeTPModifier(actor:getMod(xi.mod.STORETP))

    return math.floor(tpReturn * storeTPModifier)
end

-- Returns a PC weapon slot's TP return for a single hit.
xi.combat.tp.getSingleWeaponTPReturn = function(actor, slot)
    if not actor:isPC() then
        return 0
    end

    if actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI) then
        return 0
    end

    -- TODO: implement Zanshin check optionally?
    local delay           = actor:getBaseWeaponDelay(slot)
    local attackOutput    = xi.combat.tp.getModifiedDelayAndCanZanshin(actor, delay)
    local tpReturn        = xi.combat.tp.calculateTPReturn(actor, attackOutput.modifiedDelay)
    local storeTPModifier = xi.combat.tp.storeTPModifier(actor:getMod(xi.mod.STORETP))

    return math.floor(tpReturn * storeTPModifier)
end

-- Returns a single ranged hit's TP return
xi.combat.tp.getSingleRangedHitTPReturn = function(actor)
    if actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI) then
        return 0
    end

    local delay = actor:getBaseRangedDelay() -- there do not appear to be any delay modifiers for ranged attacks, snapshot does not seem to effect this
    if delay <= 0 then
        return 0
    end

    local storeTPModifier = xi.combat.tp.storeTPModifier(actor:getMod(xi.mod.STORETP))

    return math.floor(xi.combat.tp.calculateTPReturn(actor, delay) * storeTPModifier)
end

-- This function calculates how much TP a target(The defender) will gain upon being hit by a physical attack.
-- TODO: does Ikishoten factor into this as a bonus to baseTPGain if it procs on the hit? Needs verification.
--- @params actor CBaseEntity
--- @params target CBaseEntity
--- @params totalDamage integer
--- @params delay integer
--- @return integer
xi.combat.tp.calculateTPGainOnPhysicalDamage = function(actor, target, totalDamage, delay)
    if not actor or not target then
        return 0
    end

    if totalDamage <= 0 then
        return 0
    end

    if actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI) then
        return 0
    end

    -- TODO: does dAGI penalty work against/for Trusts/Pets? Nothing is documented for this. Currently assuming mob only.
    local attackOutput = xi.combat.tp.getModifiedDelayAndCanZanshin(actor, delay)
    local baseTPGain   = xi.combat.tp.calculateTPReturn(actor, attackOutput.modifiedDelay)

    -- TODO: unknown where/how many floor steps there are. Napkin math seems to be a single floor step, but given x/256 it's hard to tell
    -- TODO: unknown if player pets (automaton/wyvern/avatars) are affected by dAGI
    -- mob vs mob (via charm) is observed to use the (base * 1/3) formula instead of (base + 30)
    -- (base + 30) formula appears to be intentional by SE to make mobs 'more dangerous' when hit by players/pets
    return xi.combat.tp.physicalTPGain({
        baseTPGain      = baseTPGain,
        targetIsMob     = target:getObjType() == xi.objType.MOB,
        actorIsMob      = actor:getObjType() == xi.objType.MOB,
        dAGI            = actor:getStat(xi.mod.AGI) - target:getStat(xi.mod.AGI),
        inhibitTP       = target:getMod(xi.mod.INHIBIT_TP),
        storeTP         = target:getMod(xi.mod.STORETP),
        subtleBlow      = actor:getMod(xi.mod.SUBTLE_BLOW),
        subtleBlowMerit = actor:getMerit(xi.merit.SUBTLE_BLOW_EFFECT),
        subtleBlowII    = actor:getMod(xi.mod.SUBTLE_BLOW_II),
        tandemBlowBonus = xi.combat.tp.getTandemBlowBonus(actor),
    })
end

-- USED IN CORE
-- Used exclusively for blue magic.
xi.combat.tp.calculateTPGainOnMagicalDamage = function(actor, target, totalDamage)
    if not actor or not target then
        return 0
    end

    if totalDamage <= 0 then
        return 0
    end

    if actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI) then
        return 0
    end

    -- TODO: does dAGI penalty work against/for Trusts/Pets? Nothing is documented for this. Currently assuming mob only.
    -- Similar caveats to physical damage, unknown where/how many floors but seems to be one.
    return xi.combat.tp.magicalTPGain({
        targetIsMob     = target:getObjType() == xi.objType.MOB,
        dAGI            = actor:getStat(xi.mod.AGI) - target:getStat(xi.mod.AGI),
        inhibitTP       = target:getMod(xi.mod.INHIBIT_TP),
        storeTP         = target:getMod(xi.mod.STORETP),
        subtleBlow      = actor:getMod(xi.mod.SUBTLE_BLOW),
        subtleBlowMerit = actor:getMerit(xi.merit.SUBTLE_BLOW_EFFECT),
        subtleBlowII    = actor:getMod(xi.mod.SUBTLE_BLOW_II),
        tandemBlowBonus = xi.combat.tp.getTandemBlowBonus(actor),
    })
end

-- USED IN CORE
-- Calculate TP generated by spell for Occult Acumen trait
xi.combat.tp.calculateSpellTP = function(actor, spell)
    local skillType = spell:getSkillType()

    return xi.combat.tp.spellTP({
        isPC           = actor:isPC(),
        meikyoShisui   = actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI),
        skillEligible  = utils.contains(skillType, { xi.skill.ELEMENTAL_MAGIC, xi.skill.DARK_MAGIC }),
        mpCost         = spell:getMPCost(),
        occultAcumen   = actor:getMod(xi.mod.OCCULT_ACUMEN),
        storeTP        = actor:getMod(xi.mod.STORETP),
    })
end
