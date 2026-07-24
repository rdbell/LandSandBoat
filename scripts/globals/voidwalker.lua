-----------------------------------
-- The Voidwalker NM System
-----------------------------------
require('scripts/globals/mobs')
require('scripts/globals/voidwalkerpos')
-----------------------------------
xi = xi or {}
xi.voidwalker = xi.voidwalker or {}

local abyssiteKeyitems =
{
    [9] = xi.keyItem.BLACK_ABYSSITE,
    [8] = xi.keyItem.GREY_ABYSSITE,
    [7] = xi.keyItem.PURPLE_ABYSSITE,
    [6] = xi.keyItem.YELLOW_ABYSSITE,
    [5] = xi.keyItem.BROWN_ABYSSITE,
    [4] = xi.keyItem.ORANGE_ABYSSITE,
    [3] = xi.keyItem.BLUE_ABYSSITE,
    [2] = xi.keyItem.COLORFUL_ABYSSITE,
    [1] = xi.keyItem.CLEAR_ABYSSITE,
}

local abyssiteMessage =
{
    [xi.keyItem.CLEAR_ABYSSITE]    = 0,
    [xi.keyItem.COLORFUL_ABYSSITE] = 1,
    [xi.keyItem.BLUE_ABYSSITE]     = 2,
    [xi.keyItem.ORANGE_ABYSSITE]   = 2,
    [xi.keyItem.BROWN_ABYSSITE]    = 2,
    [xi.keyItem.YELLOW_ABYSSITE]   = 2,
    [xi.keyItem.GREY_ABYSSITE]     = 2,
    [xi.keyItem.BLACK_ABYSSITE]    = 3
}

local function getCurrentKIsBitsFromPlayer(player)
    local results = 0

    for i, keyitem in ipairs(abyssiteKeyitems) do
        local currentBit = 0
        if player:hasKeyItem(keyitem) then
            currentBit = 1
        end

        results = results + bit.lshift(currentBit, i - 1)
    end

    return results
end

local function getCurrentKIsFromPlayer(player)
    local results = {}

    for i, keyitem in ipairs(abyssiteKeyitems) do
        if player:hasKeyItem(keyitem) then
            table.insert(results, keyitem)
        end
    end

    return results
end

local function getMobsFromAbyssites(zoneId, abyssites)
    local results = {}

    for i, keyitem in ipairs(abyssites) do
        if
            zones[zoneId] and
            zones[zoneId].mob and
            zones[zoneId].mob.VOIDWALKER[keyitem]
        then
            for _, mobId in ipairs(zones[zoneId].mob.VOIDWALKER[keyitem]) do
                local mob = GetMobByID(mobId)

                if mob and mob:isAlive() and mob:getLocalVar('[VoidWalker]PopedBy') == 0 then
                    table.insert(results, { mobId = mobId, keyItem = keyitem })
                end
            end
        end
    end

    return results
end

local function removeMobIdFromPos(zoneId, mobId)
    for i, pos in ipairs(xi.voidwalker.pos[zoneId]) do
        if pos.mobId == mobId then
            xi.voidwalker.pos[zoneId][i].mobId = nil
        end
    end
end

local function searchEmptyPos(zoneId)
    local maxPos     = #xi.voidwalker.pos[zoneId]
    local pos        = math.random(1, maxPos)
    local currentPos = xi.voidwalker.pos[zoneId][pos]

    if currentPos.mobId == nil then
        return pos
    else
        return searchEmptyPos(zoneId)
    end
end

local function setRandomPos(zoneId, mobId)
    local mob = GetMobByID(mobId)

    if
        not mob or
        not xi.voidwalker.pos[zoneId]
    then
        return
    end

    local pos = searchEmptyPos(zoneId)

    xi.voidwalker.pos[zoneId][pos].mobId = mobId
    local vPos                           = xi.voidwalker.pos[zoneId][pos].pos

    mob:setSpawn(vPos[1], vPos[2], vPos[3])
    mob:setPos(vPos[1], vPos[2], vPos[3])
end

xi.voidwalker.nearestMob = function(mobs, distanceForMob)
    local results = {}

    if not mobs or not distanceForMob then
        return nil
    end

    for _, v in ipairs(mobs) do
        local distance = distanceForMob(v.mobId)

        table.insert(results, { mobId = v.mobId, keyItem = v.keyItem, distance = distance })
    end

    table.sort(results, function(a, b)
        return a.distance < b.distance
    end)

    if #results > 0 then
        return results[1]
    else
        return nil
    end
end

xi.voidwalker.healingRangeOutcome = function(distance)
    if distance <= 4 then
        return 'spawn'
    elseif distance >= 300 then
        return 'too_far'
    end

    return 'hint'
end

xi.voidwalker.shouldUpgradeOriginalPopper = function(isKiller, popperExists, popperInAlliance, popperHasKeyItem)
    return isKiller and popperExists and not popperInAlliance and not popperHasKeyItem
end

-- Pure onMobDeath alliance scan: reports whether the player who popped the NM
-- is still in the killer's alliance. memberIds is the killer's alliance roster.
xi.voidwalker.popperInAlliance = function(memberIds, popperId, popperExists)
    if not popperExists then
        return false
    end

    for _, memberId in ipairs(memberIds) do
        if memberId == popperId then
            return true
        end
    end

    return false
end

-- Pure onMobDeath dispatch: which of the two independent checkUpgrade rolls
-- fire, in upstream order. The original popper is only considered when the
-- reporting player is the killer; the killer's own roll is always considered.
xi.voidwalker.mobDeathUpgradePlan = function(
    isKiller,
    popperExists,
    popperInAlliance,
    popperHasNextKeyItem,
    killerHasPopKeyItem,
    killerHasNextKeyItem
)
    local upgradePopper = false

    if isKiller then
        upgradePopper = xi.voidwalker.shouldUpgradeOriginalPopper(
            isKiller,
            popperExists,
            popperInAlliance,
            popperHasNextKeyItem
        ) or false
    end

    return
    {
        upgradePopper = upgradePopper,
        upgradeKiller = xi.voidwalker.shouldUpgradeKiller(killerHasPopKeyItem, killerHasNextKeyItem) or false,
    }
end

xi.voidwalker.shouldUpgradeKiller = function(killerHasPopKeyItem, killerHasNextKeyItem)
    return killerHasPopKeyItem and not killerHasNextKeyItem
end

xi.voidwalker.shouldConsumePopAbyssite = function(keyItem)
    return keyItem ~= xi.keyItem.CLEAR_ABYSSITE and keyItem ~= xi.keyItem.COLORFUL_ABYSSITE
end

xi.voidwalker.upgradeMessageKind = function(currentKeyItem, nextKeyItem)
    if currentKeyItem == xi.keyItem.CLEAR_ABYSSITE then
        return 'upgrade_1'
    elseif currentKeyItem == xi.keyItem.COLORFUL_ABYSSITE then
        return 'upgrade_2'
    elseif nextKeyItem == xi.keyItem.BLACK_ABYSSITE then
        return 'obtain'
    end
end

xi.voidwalker.npcUpdateOutcome = function(csid, option, hasGil, hasClearAbyssite)
    if csid ~= 10120 or bit.band(option, 0xF) ~= 3 then
        return nil
    elseif not hasGil then
        return 'no_gil'
    elseif hasClearAbyssite then
        return 'has_clear_abyssite'
    end

    return 'purchase_available'
end

xi.voidwalker.shouldDespawnOnFight = function(isSpawned, now, poppedAt, distance)
    return isSpawned and (now > (poppedAt + 7200) or distance > 25)
end

xi.voidwalker.npcFinishAction = function(csid, option)
    if csid ~= 10120 then
        return nil
    end

    local opt = bit.band(option, 0xF)
    if opt == 1 then
        return { kind = 'buy_clear', keyItem = abyssiteKeyitems[1] }
    elseif opt == 2 then
        return { kind = 'remove_abyssite', keyItem = abyssiteKeyitems[bit.rshift(option, 4)] }
    end
end

local getNearestMob = function(player, mobs)
    return xi.voidwalker.nearestMob(mobs, function(mobId)
        return player:checkDistance(GetMobByID(mobId))
    end)
end

local spawnModifierPlans =
{
    ['Krabkatoa'] =
    {
        { kind = 'status', effect = xi.effect.REGAIN, power = 10 },
        { kind = 'mod', mod = xi.mod.DOUBLE_ATTACK, value = 10 },
    },
    ['Tammuz'] =
    {
        { kind = 'status', effect = xi.effect.MIGHTY_STRIKES, power = 1 },
    },
    ['Erebus'] =
    {
        { kind = 'immunity', immunity = xi.immunity.GRAVITY },
        { kind = 'immunity', immunity = xi.immunity.BIND },
    },
    ['Raker_Bee'] =
    {
        { kind = 'immunity', immunity = xi.immunity.GRAVITY },
        { kind = 'immunity', immunity = xi.immunity.BIND },
    },
    ['Gjenganger'] =
    {
        { kind = 'immunity', immunity = xi.immunity.STUN },
    },
}

xi.voidwalker.spawnModifierPlan = function(mobName)
    return spawnModifierPlans[mobName]
end

local mobSkillEveryHPPPlans =
{
    ['Capricornus'] = { every = 20, start = 80, mobSkill = xi.mobSkill.MIGHTY_STRIKES_1, absentEffect = xi.effect.MIGHTY_STRIKES },
    ['Yacumama']    = { every = 20, start = 80, mobSkill = xi.mobSkill.HUNDRED_FISTS_1, absentEffect = xi.effect.HUNDRED_FISTS },
    ['Shoggoth']    = { every = 20, start = 80, mobSkill = xi.mobSkill.CHAINSPELL_1, absentEffect = xi.effect.CHAINSPELL },
    ['Blobdingnag'] = { every = 20, start = 82, mobSkill = xi.mobSkill.CYTOKINESIS },
    ['Farruca_Fly'] = { every = 20, start = 80, mobSkill = xi.mobSkill.PERFECT_DODGE_1, absentEffect = xi.effect.PERFECT_DODGE },
    ['Skuld']       = { every = 20, start = 80, mobSkill = xi.mobSkill.CHAINSPELL_1, absentEffect = xi.effect.CHAINSPELL },
    ['Dawon']       = { every = 20, start = 80, mobSkill = xi.mobSkill.PERFECT_DODGE_1, absentEffect = xi.effect.PERFECT_DODGE },
}

xi.voidwalker.mobSkillEveryHPPPlan = function(mobName)
    return mobSkillEveryHPPPlans[mobName]
end

local randomMobSkillPlans =
{
    ['Lamprey_Lord'] = { chance = 10, between = 60, statusGate = xi.effect.BLOOD_WEAPON, mobSkill = xi.mobSkill.BLOOD_WEAPON_1 },
    ['Jyeshtha']     = { chance = 30, between = 60, statusGate = xi.mobSkill.MIGHTY_STRIKES_1, mobSkill = xi.mobSkill.MIGHTY_STRIKES_1 },
    ['Erebus']       = { chance = 30, between = 60, statusGate = xi.effect.BLOOD_WEAPON, mobSkill = xi.mobSkill.BLOOD_WEAPON_1 },
    ['Feuerunke']    = { chance = 30, between = 60, statusGate = xi.effect.HUNDRED_FISTS, mobSkill = xi.mobSkill.HUNDRED_FISTS_1 },
}

xi.voidwalker.randomMobSkillPlan = function(mobName)
    return randomMobSkillPlans[mobName]
end

local resetLocalVarPlan =
{
    '[VoidWalker]PopedBy',
    '[VoidWalker]checkPopedBy',
    '[VoidWalker]PopedWith',
    '[VoidWalker]PopedAt',
    'MOBSKILL_USE',
    'MOBSKILL_TIME',
}

xi.voidwalker.resetLocalVars = function()
    return resetLocalVarPlan
end

xi.voidwalker.spawnPresentationPlan = function()
    return { status = xi.status.INVISIBLE, hideHP = true, hideName = true, untargetable = true }
end

xi.voidwalker.shouldHandleHealing = function(voidwalkerEnabled, abyssiteCount, hasVoidwalkerMobs)
    return voidwalkerEnabled and abyssiteCount > 0 and hasVoidwalkerMobs
end

-- Pure onHealing branch selection once the nearest Voidwalker mob is resolved.
-- nearest is the { mobId, keyItem, distance } row from nearestMob, or nil when
-- the zone had no eligible candidate. firstAbyssite is abyssites[1], reported
-- with VOIDWALKER_NO_MOB. diffx/diffz are the mob-minus-player offsets used for
-- the hint direction.
xi.voidwalker.healingOutcomePlan = function(nearest, firstAbyssite, diffx, diffz)
    if not nearest then
        return { kind = 'no_mob', keyItem = firstAbyssite }
    end

    local outcome = xi.voidwalker.healingRangeOutcome(nearest.distance)

    if outcome == 'spawn' then
        local consume = xi.voidwalker.shouldConsumePopAbyssite(nearest.keyItem)

        return
        {
            kind     = 'spawn',
            mobId    = nearest.mobId,
            keyItem  = nearest.keyItem,
            distance = nearest.distance,
            consume  = consume,
            -- Upstream only reveals HP on the retained-abyssite path; a consumed
            -- abyssite leaves the mob's HP bar hidden.
            showHP   = not consume,
        }
    elseif outcome == 'too_far' then
        return { kind = 'too_far', keyItem = nearest.keyItem, distance = nearest.distance }
    end

    return
    {
        kind      = 'hint',
        keyItem   = nearest.keyItem,
        distance  = nearest.distance,
        tier      = abyssiteMessage[nearest.keyItem],
        direction = xi.voidwalker.direction(diffx, diffz),
    }
end

xi.voidwalker.shouldCapricornusUseRecoilDive = function(hasMightyStrikes, isBusy)
    return hasMightyStrikes and not isBusy
end

xi.voidwalker.shouldResetJyeshthaMobSkillUse = function(mobSkillUse, hasMightyStrikes)
    return mobSkillUse == 1 and not hasMightyStrikes
end

xi.voidwalker.shouldErebusApplyHundredFists = function(hasBloodWeapon, hasHundredFists)
    return hasBloodWeapon and not hasHundredFists
end

xi.voidwalker.direction = function(diffx, diffz)
    local tan       = math.atan(diffz / diffx)
    local degree    = math.deg(tan)

    if degree < 0 then
        degree = degree * -1
    end

    local minDegree = 20
    local maxDegree = 70

    -- Degree >= 70
    if degree >= maxDegree then
        if diffz >= 0 then
            return 6
        else
            return 2
        end

    -- Degree <= 20
    elseif degree <= minDegree then
        if diffx >= 0 then
            return 0
        else
            return 4
        end

    -- Degree between 20 and 70
    else
        if diffz >= 0 then
            if diffx >= 0 then
                return 7
            else
                return 5
            end
        else
            if diffx >= 0 then
                return 1
            else
                return 3
            end
        end
    end
end

-----------------------------------
-- check keyitem upgrade
-----------------------------------

-- Pure checkUpgrade body once the host resolves the zone match, the
-- math.random(1, 10) roll, and the player's hold on the current abyssite.
-- Returns nil when the upgrade does not fire. delKeyItem/addKeyItem/message are
-- absent when that step is skipped; note that a winning roll still spends the
-- current abyssite even when there is no next one to grant.
xi.voidwalker.keyItemUpgradePlan = function(sameZone, roll, currentKeyItem, hasCurrentKeyItem, nextKeyItem)
    if
        not sameZone or
        roll ~= 5
    then
        return nil
    end

    local plan = { currentKeyItem = currentKeyItem }

    if hasCurrentKeyItem then
        plan.delKeyItem = currentKeyItem
    end

    if nextKeyItem then
        plan.addKeyItem = nextKeyItem
        plan.message    = xi.voidwalker.upgradeMessageKind(currentKeyItem, nextKeyItem)
    end

    return plan
end

local function checkUpgrade(player, mob, nextKeyItem)
    -- Short-circuit before rolling: the plan repeats the zone gate for its own
    -- tests, but only a same-zone player may consume from the shared RNG.
    if
        not player or
        mob:getZoneID() ~= player:getZoneID()
    then
        return
    end

    local currentKeyItem = mob:getLocalVar('[VoidWalker]PopedWith')
    local plan           = xi.voidwalker.keyItemUpgradePlan(
        true,
        math.random(1, 10),
        currentKeyItem,
        player:hasKeyItem(currentKeyItem),
        nextKeyItem
    )

    if not plan then
        return
    end

    local zoneTextTable = zones[mob:getZoneID()].text

    if plan.delKeyItem then
        player:delKeyItem(plan.delKeyItem)
    end

    if plan.addKeyItem then
        player:addKeyItem(plan.addKeyItem)

        if plan.message == 'upgrade_1' then
            player:messageSpecial(zoneTextTable.VOIDWALKER_UPGRADE_KI_1, plan.currentKeyItem, plan.addKeyItem)
        elseif plan.message == 'upgrade_2' then
            player:messageSpecial(zoneTextTable.VOIDWALKER_UPGRADE_KI_2, plan.currentKeyItem, plan.addKeyItem)
        elseif plan.message == 'obtain' then
            player:messageSpecial(zoneTextTable.VOIDWALKER_OBTAIN_KI, plan.addKeyItem)
        end
    end
end

-----------------------------------
-- NPC Assai Nybaem
-----------------------------------
xi.voidwalker.npcOnTrigger = function(player, npc)
    if xi.settings.main.ENABLE_VOIDWALKER ~= 1 then
        return
    end

    local currentKIS = getCurrentKIsBitsFromPlayer(player)
    player:startEvent(10120, currentKIS)
end

xi.voidwalker.npcOnEventUpdate = function(player, csid, option, npc)
    local outcome = xi.voidwalker.npcUpdateOutcome(
        csid,
        option,
        player:getGil() >= 1000,
        player:hasKeyItem(xi.keyItem.CLEAR_ABYSSITE)
    )

    if outcome == 'no_gil' then
        player:updateEvent(3)
    elseif outcome == 'has_clear_abyssite' then
        player:updateEvent(2)
    elseif outcome == 'purchase_available' then
        player:updateEvent(1)
    end
end

xi.voidwalker.npcOnEventFinish = function(player, csid, option, npc)
    local action = xi.voidwalker.npcFinishAction(csid, option)
    if action and action.kind == 'buy_clear' then
        local msg = zones[xi.zone.RULUDE_GARDENS]
        player:delGil(1000)
        player:addKeyItem(action.keyItem)
        player:messageSpecial(msg.text.KEYITEM_OBTAINED, action.keyItem)
    elseif action and action.kind == 'remove_abyssite' then
        player:delKeyItem(action.keyItem)
    end
end

-----------------------------------
-- Zone On Init
-----------------------------------
xi.voidwalker.zoneOnInit = function(zone)
    local zoneId         = zone:getID()
    local voidwalkerMobs = zones[zoneId].mob.VOIDWALKER

    for ki, mobs in pairs(voidwalkerMobs) do
        for _, mob in pairs(mobs) do
            setRandomPos(zoneId, mob)
        end
    end
end

local function doMobSkillEveryHPP(mob, every, start, mobskill, condition)
    local mobhpp = mob:getHPP()

    if
        mobhpp <= start and
        condition
    then
        local mobHppModulo   = mobhpp % every
        local startHppModulo = start % every
        local isSame         = startHppModulo == mobHppModulo

        if
            isSame and
            mob:getLocalVar('MOB_SKILL_' .. mobhpp) == 0
        then
            mob:useMobAbility(mobskill)
            mob:setLocalVar('MOB_SKILL_' .. mobhpp, 1)
        end
    end
end

local function doMobSkillPlan(mob, mobName)
    local plan = xi.voidwalker.mobSkillEveryHPPPlan(mobName)
    if not plan then
        return
    end

    local condition = not plan.absentEffect or not mob:hasStatusEffect(plan.absentEffect)
    doMobSkillEveryHPP(mob, plan.every, plan.start, plan.mobSkill, condition)
end

local function randomly(mob, chance, between, effect, skill)
    if
        math.random(0, 100) <= chance and
        not mob:hasStatusEffect(effect) and
        GetSystemTime() > (mob:getLocalVar('MOBSKILL_TIME') + between)
    then
        mob:setLocalVar('MOBSKILL_USE', 1)
        mob:setLocalVar('MOBSKILL_TIME', GetSystemTime())
        mob:useMobAbility(skill)
    end
end

local function doRandomMobSkillPlan(mob, mobName)
    local plan = xi.voidwalker.randomMobSkillPlan(mobName)
    if plan then
        randomly(mob, plan.chance, plan.between, plan.statusGate, plan.mobSkill)
    end
end

local function DespawnPet(mob)
    local zoneId = mob:getZoneID()
    local mobId  = mob:getID()

    if zones[zoneId].pet and zones[zoneId].pet[mobId] then
        local petIds = zones[zoneId].pet[mobId]

        for i, petId in ipairs(petIds) do
            local pet = GetMobByID(petId)

            if pet then
                DespawnMob(petId)
                pet:setSpawn(mob:getXPos(), mob:getYPos(), mob:getZPos())
                pet:setPos(mob:getXPos(), mob:getYPos(), mob:getZPos())
            end
        end
    end
end

local function resetMobLocalVars(mob)
    for _, name in ipairs(xi.voidwalker.resetLocalVars()) do
        mob:setLocalVar(name, 0)
    end
end

local mobFightSpecialStep =
{
    ['Capricornus'] = 'recoil_dive',
    ['Jyeshtha']    = 'mob_skill_use_reset',
    ['Erebus']      = 'hundred_fists',
}

-- Pure onMobFight mixin dispatch. The scheduled and random skill steps are
-- derived from the shared plan catalogs, so the roster of mobs with an
-- onMobFight mixin stays in one place. Returns nil for mobs without one.
xi.voidwalker.mobFightMixinPlan = function(mobName)
    local special = mobFightSpecialStep[mobName]
    local fixed   = xi.voidwalker.mobSkillEveryHPPPlan(mobName) ~= nil
    local random  = xi.voidwalker.randomMobSkillPlan(mobName) ~= nil

    if
        not fixed and
        not random and
        not special
    then
        return nil
    end

    return
    {
        fixedHPPSkill = fixed,
        randomSkill   = random,
        special       = special,
    }
end

-----------------------------------
-- Mob On Init
-----------------------------------
xi.voidwalker.onMobInitialize = function(mob)
end

xi.voidwalker.onMobSpawn = function(mob)
    local mobName = mob:getName()
    local presentation = xi.voidwalker.spawnPresentationPlan()
    mob:setStatus(presentation.status)
    mob:hideHP(presentation.hideHP)
    mob:hideName(presentation.hideName)
    mob:setUntargetable(presentation.untargetable)
    local modifiers = xi.voidwalker.spawnModifierPlan(mobName)
    if modifiers then
        for _, modifier in ipairs(modifiers) do
            if modifier.kind == 'status' then
                mob:addStatusEffect(modifier.effect, { power = modifier.power, origin = mob })
            elseif modifier.kind == 'mod' then
                mob:addMod(modifier.mod, modifier.value)
            elseif modifier.kind == 'immunity' then
                mob:addImmunity(modifier.immunity)
            end
        end
    end
end

xi.voidwalker.onMobFight = function(mob, target)
    local mobName = mob:getName()
    local mixin   = xi.voidwalker.mobFightMixinPlan(mobName)

    if mixin then
        if mixin.fixedHPPSkill then
            doMobSkillPlan(mob, mobName)
        end

        if mixin.randomSkill then
            doRandomMobSkillPlan(mob, mobName)
        end

        if mixin.special == 'recoil_dive' then
            if xi.voidwalker.shouldCapricornusUseRecoilDive(
                mob:hasStatusEffect(xi.effect.MIGHTY_STRIKES),
                xi.combat.behavior.isEntityBusy(mob)
            ) then
                mob:useMobAbility(xi.mobSkill.RECOIL_DIVE_1)
            end
        elseif mixin.special == 'mob_skill_use_reset' then
            if xi.voidwalker.shouldResetJyeshthaMobSkillUse(
                mob:getLocalVar('MOBSKILL_USE'),
                mob:hasStatusEffect(xi.effect.MIGHTY_STRIKES)
            ) then
                mob:setLocalVar('MOBSKILL_USE', 0)
            end
        elseif mixin.special == 'hundred_fists' then
            if xi.voidwalker.shouldErebusApplyHundredFists(
                mob:hasStatusEffect(xi.effect.BLOOD_WEAPON),
                mob:hasStatusEffect(xi.effect.HUNDRED_FISTS)
            ) then
                mob:addStatusEffect(xi.effect.HUNDRED_FISTS, { power = 1, duration = 30, origin = mob })
            end
        end
    end

    local poptime = mob:getLocalVar('[VoidWalker]PopedAt')
    local now     = GetSystemTime()

    if xi.voidwalker.shouldDespawnOnFight(mob:isSpawned(), now, poptime, mob:checkDistance(target)) then
        local zoneTextTable = zones[mob:getZoneID()].text

        target:messageSpecial(zoneTextTable.VOIDWALKER_DESPAWN)
        DespawnMob(mob:getID())
    end
end

xi.voidwalker.onMobDisengage = function(mob)
    resetMobLocalVars(mob)
    DespawnPet(mob)
    mob:setStatus(xi.status.INVISIBLE)
    mob:hideHP(true)
    mob:hideName(true)
    mob:setUntargetable(true)
end

xi.voidwalker.onMobDespawn = function(mob)
    local zoneId = mob:getZoneID()
    local mobId  = mob:getID()

    removeMobIdFromPos(zoneId, mobId)
    setRandomPos(zoneId, mobId)
    resetMobLocalVars(mob)
    DespawnPet(mob)
end

xi.voidwalker.onMobDeath = function(mob, player, optParams, keyItem)
    if player then
        local popkeyitem  = mob:getLocalVar('[VoidWalker]PopedWith')
        local playerpoped = nil
        local inAlliance  = false

        if optParams.isKiller then
            playerpoped = GetPlayerByID(mob:getLocalVar('[VoidWalker]PopedBy'))

            local memberIds = {}
            for _, member in pairs(player:getAlliance()) do
                table.insert(memberIds, member:getID())
            end

            inAlliance = xi.voidwalker.popperInAlliance(
                memberIds,
                playerpoped and playerpoped:getID(),
                playerpoped ~= nil
            )
        end

        local plan = xi.voidwalker.mobDeathUpgradePlan(
            optParams.isKiller,
            playerpoped ~= nil,
            inAlliance,
            playerpoped and playerpoped:hasKeyItem(keyItem),
            player:hasKeyItem(popkeyitem),
            player:hasKeyItem(keyItem)
        )

        if plan.upgradePopper then
            checkUpgrade(playerpoped, mob, keyItem)
        end

        if plan.upgradeKiller then
            checkUpgrade(player, mob, keyItem)
        end
    end
end

-----------------------------------
-- onHealing : trigg when player /heal
-----------------------------------
xi.voidwalker.onHealing = function(player)
    if xi.settings.main.ENABLE_VOIDWALKER ~= 1 then
        return
    end

    local zoneId        = player:getZoneID()
    local zoneTextTable = zones[zoneId].text
    local abyssites     = getCurrentKIsFromPlayer(player)

    if not xi.voidwalker.shouldHandleHealing(
        true,
        #abyssites,
        zones[zoneId].mob and zones[zoneId].mob.VOIDWALKER
    ) then
        return
    end

    local mobs       = getMobsFromAbyssites(zoneId, abyssites)
    local mobNearest = getNearestMob(player, mobs)
    local mob        = mobNearest and GetMobByID(mobNearest.mobId)
    local diffx      = 0
    local diffz      = 0

    -- The hint branch needs the player-to-mob offset, which the plan cannot
    -- derive on its own, so resolve it up front for any nearest candidate.
    if mob then
        local posPlayer = player:getPos()
        local posMob    = mob:getPos()

        diffx = posMob.x - posPlayer.x
        diffz = posMob.z - posPlayer.z
    end

    local plan = xi.voidwalker.healingOutcomePlan(mobNearest, abyssites[1], diffx, diffz)

    if plan.kind == 'no_mob' then
        player:messageSpecial(zoneTextTable.VOIDWALKER_NO_MOB, plan.keyItem)
    elseif plan.kind == 'spawn' then
        if not mob then
            return
        end

        mob:setLocalVar('[VoidWalker]PopedBy', player:getID())
        mob:setLocalVar('[VoidWalker]PopedWith', plan.keyItem)
        mob:setLocalVar('[VoidWalker]PopedAt', GetSystemTime())

        if plan.consume then
            player:delKeyItem(plan.keyItem)
            player:messageSpecial(zoneTextTable.VOIDWALKER_BREAK_KI, plan.keyItem)
        else
            player:messageSpecial(zoneTextTable.VOIDWALKER_SPAWN_MOB)
        end

        if plan.showHP then
            mob:hideHP(false)
        end

        mob:hideName(false)
        mob:setUntargetable(false)
        mob:setStatus(xi.status.UPDATE)
        mob:updateClaim(player)

    elseif plan.kind == 'too_far' then
        player:messageSpecial(zoneTextTable.VOIDWALKER_MOB_TOO_FAR, plan.keyItem)

    else
        player:messageSpecial(zoneTextTable.VOIDWALKER_MOB_HINT, plan.tier, plan.direction, plan.distance, plan.keyItem)
    end
end
