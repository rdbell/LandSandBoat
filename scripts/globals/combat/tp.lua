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

-- Bonus subtle blow II from Tandem Blow (BST trait)
xi.combat.tp.getTandemBlowBonus = function(actor)
    local tandemBlowBonus = 0
    if actor:isTandemActive() then
        if actor:getMaster() ~= nil and actor:getMaster():isPC() then
            tandemBlowBonus = actor:getMaster():getMod(xi.mod.TANDEM_BLOW_POWER)
        else
            tandemBlowBonus = actor:getMod(xi.mod.TANDEM_BLOW_POWER)
        end
    end

    return tandemBlowBonus
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

    local storeTPModifier = 1 + actor:getMod(xi.mod.STORETP) / 100

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
    local storeTPModifier = 1 + actor:getMod(xi.mod.STORETP) / 100

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

    local storeTPModifier = 1 + actor:getMod(xi.mod.STORETP) / 100

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
    local attackOutput       = xi.combat.tp.getModifiedDelayAndCanZanshin(actor, delay)
    local baseTPGain         = xi.combat.tp.calculateTPReturn(actor, attackOutput.modifiedDelay)
    local dAGI               = actor:getStat(xi.mod.AGI) - target:getStat(xi.mod.AGI)
    local inhibitTPModifier  = (100 - target:getMod(xi.mod.INHIBIT_TP)) / 100                    -- no known cap: https://www.bg-wiki.com/ffxi/Monster_TP_gain#Inhibit_TP
    local dAGIModifier       = utils.clamp(200 - (dAGI + 30) / 200, 0.5, 1)                      -- 50% reduction at +70 dAGI: https://www.bg-wiki.com/ffxi/Monster_TP_gain
    local subtleBlowMerits   = actor:getMerit(xi.merit.SUBTLE_BLOW_EFFECT)
    local subtleBlowI        = math.min(actor:getMod(xi.mod.SUBTLE_BLOW) + subtleBlowMerits, 50) -- cap of 50% https://www.bg-wiki.com/ffxi/Subtle_Blow
    local tandemBlowBonus    = xi.combat.tp.getTandemBlowBonus(actor)
    local subtleBlowII       = actor:getMod(xi.mod.SUBTLE_BLOW_II) + tandemBlowBonus             -- no known cap
    local subtleBlowModifier = math.max((100 - subtleBlowI + subtleBlowII) / 100, 0.25)          -- combined cap of 75% reduction: https://www.bg-wiki.com/ffxi/Subtle_Blow
    local storeTPModifier    = 1 + target:getMod(xi.mod.STORETP) / 100

    -- TODO: unknown where/how many floor steps there are. Napkin math seems to be a single floor step, but given x/256 it's hard to tell
    -- TODO: unknown if player pets (automaton/wyvern/avatars) are affected by dAGI

    -- mob vs mob (via charm) is observed to use the (base * 1/3) formula instead of (base + 30)
    -- (base + 30) formula appears to be intentional by SE to make mobs 'more dangerous' when hit by players/pets
    if
        target:getObjType() == xi.objType.MOB and
        actor:getObjType() ~= xi.objType.MOB
    then
        -- +30 sourced from http://wiki.ffo.jp/html/2621.html and tested in game
        return math.floor((baseTPGain + 30) * inhibitTPModifier * dAGIModifier * subtleBlowModifier * storeTPModifier)
    else
        -- 1/3rd sourced from https://www.bg-wiki.com/ffxi/Tactical_Points and tested in game
        return math.floor(baseTPGain * inhibitTPModifier * subtleBlowModifier * storeTPModifier * (1 / 3))
    end
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
    local dAGI               = actor:getStat(xi.mod.AGI) - target:getStat(xi.mod.AGI)
    local inhibitTPModifier  = (100 - target:getMod(xi.mod.INHIBIT_TP)) / 100                    -- no known cap: https://www.bg-wiki.com/ffxi/Monster_TP_gain#Inhibit_TP
    local dAGIModifier       = utils.clamp(200 - (dAGI + 30) / 200, 0.5, 1)                      -- 50% reduction at +70 dAGI: https://www.bg-wiki.com/ffxi/Monster_TP_gain
    local subtleBlowMerits   = actor:getMerit(xi.merit.SUBTLE_BLOW_EFFECT)
    local subtleBlowI        = math.min(actor:getMod(xi.mod.SUBTLE_BLOW) + subtleBlowMerits, 50) -- cap of 50% https://www.bg-wiki.com/ffxi/Subtle_Blow
    local tandemBlowBonus    = xi.combat.tp.getTandemBlowBonus(actor)
    local subtleBlowII       = actor:getMod(xi.mod.SUBTLE_BLOW_II) + tandemBlowBonus             -- no known cap
    local subtleBlowModifier = math.max((100 - subtleBlowI + subtleBlowII) / 100, 0.25)          -- combined cap of 75% reduction: https://www.bg-wiki.com/ffxi/Subtle_Blow
    local storeTPModifier    = 1 + target:getMod(xi.mod.STORETP) / 100

    -- Similar caveats to above for physical damage, unknown where/how many floors but seems to be one.
    if target:getObjType() == xi.objType.MOB then
        return math.floor(100 * inhibitTPModifier * dAGIModifier * subtleBlowModifier * storeTPModifier) -- 100 sourced from testing & https://www.bg-wiki.com/ffxi/Monster_TP_gain#TP_gained_from_Magical_Damage
    else
        return math.floor(50 * inhibitTPModifier * subtleBlowModifier * storeTPModifier)                 -- 50 sourced from testing & https://www.bg-wiki.com/ffxi/Tactical_Points#Getting_hit_for_more_than_0_damage
    end
end

-- USED IN CORE
-- Calculate TP generated by spell for Occult Acumen trait
xi.combat.tp.calculateSpellTP = function(actor, spell)
    if not actor:isPC() then
        return 0
    end

    if actor:hasStatusEffect(xi.effect.MEIKYO_SHISUI) then
        return 0
    end

    if not utils.contains(spell:getSkillType(), { xi.skill.ELEMENTAL_MAGIC, xi.skill.DARK_MAGIC }) then
        return 0
    end

    local occultAcumenModifier = actor:getMod(xi.mod.OCCULT_ACUMEN) / 100
    local storeTPModifier      = 1 + actor:getMod(xi.mod.STORETP) / 100

    return math.floor(spell:getMPCost() * occultAcumenModifier * storeTPModifier)
end
