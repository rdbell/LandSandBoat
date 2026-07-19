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

local getDirection = function(player, mob, distance)
    local posPlayer = player:getPos()
    local posMob    = mob:getPos()
    local diffx     = posMob.x - posPlayer.x
    local diffz     = posMob.z - posPlayer.z

    return xi.voidwalker.direction(diffx, diffz)
end

-----------------------------------
-- check keyitem upgrade
-----------------------------------
local function checkUpgrade(player, mob, nextKeyItem)
    if
        player and
        mob:getZoneID() == player:getZoneID()
    then
        local zoneTextTable  = zones[mob:getZoneID()].text
        local currentKeyItem = mob:getLocalVar('[VoidWalker]PopedWith')
        local rand           = math.random(1, 10)

        if rand == 5 then
            if player:hasKeyItem(currentKeyItem) then
                player:delKeyItem(currentKeyItem)
            end

            if nextKeyItem then
                player:addKeyItem(nextKeyItem)

                local messageKind = xi.voidwalker.upgradeMessageKind(currentKeyItem, nextKeyItem)
                if messageKind == 'upgrade_1' then
                    player:messageSpecial(zoneTextTable.VOIDWALKER_UPGRADE_KI_1, currentKeyItem, nextKeyItem)
                elseif messageKind == 'upgrade_2' then
                    player:messageSpecial(zoneTextTable.VOIDWALKER_UPGRADE_KI_2, currentKeyItem, nextKeyItem)
                elseif messageKind == 'obtain' then
                    player:messageSpecial(zoneTextTable.VOIDWALKER_OBTAIN_KI, nextKeyItem)
                end
            end
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

local mixinByMobName =
{
    ['Capricornus'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 80, xi.mobSkill.MIGHTY_STRIKES_1, not mob:hasStatusEffect(xi.effect.MIGHTY_STRIKES))
        if xi.voidwalker.shouldCapricornusUseRecoilDive(
            mob:hasStatusEffect(xi.effect.MIGHTY_STRIKES),
            xi.combat.behavior.isEntityBusy(mob)
        ) then
            mob:useMobAbility(xi.mobSkill.RECOIL_DIVE_1)
        end
    end,

    ['Yacumama'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 80, xi.mobSkill.HUNDRED_FISTS_1, not mob:hasStatusEffect(xi.effect.HUNDRED_FISTS))
    end,

    ['Lamprey_Lord'] = function(mob)
        randomly(mob, 10, 60, xi.effect.BLOOD_WEAPON, xi.mobSkill.BLOOD_WEAPON_1)
    end,

    ['Shoggoth'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 80, xi.mobSkill.CHAINSPELL_1, not mob:hasStatusEffect(xi.effect.CHAINSPELL))
    end,

    ['Jyeshtha'] = function(mob)
        randomly(mob, 30, 60, xi.mobSkill.MIGHTY_STRIKES_1, xi.mobSkill.MIGHTY_STRIKES_1)
        if xi.voidwalker.shouldResetJyeshthaMobSkillUse(
            mob:getLocalVar('MOBSKILL_USE'),
            mob:hasStatusEffect(xi.effect.MIGHTY_STRIKES)
        ) then
            mob:setLocalVar('MOBSKILL_USE', 0)
        end
    end,

    ['Blobdingnag'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 82, xi.mobSkill.CYTOKINESIS, true)
    end,

    ['Farruca_Fly'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 80, xi.mobSkill.PERFECT_DODGE_1, not mob:hasStatusEffect(xi.effect.PERFECT_DODGE))
    end,

    ['Skuld'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 80, xi.mobSkill.CHAINSPELL_1, not mob:hasStatusEffect(xi.effect.CHAINSPELL))
    end,

    ['Erebus'] = function(mob)
        randomly(mob, 30, 60, xi.effect.BLOOD_WEAPON, xi.mobSkill.BLOOD_WEAPON_1)
        if xi.voidwalker.shouldErebusApplyHundredFists(
            mob:hasStatusEffect(xi.effect.BLOOD_WEAPON),
            mob:hasStatusEffect(xi.effect.HUNDRED_FISTS)
        ) then
            mob:addStatusEffect(xi.effect.HUNDRED_FISTS, { power = 1, duration = 30, origin = mob })
        end
    end,

    ['Feuerunke'] = function(mob)
        randomly(mob, 30, 60, xi.effect.HUNDRED_FISTS, xi.mobSkill.HUNDRED_FISTS_1)
    end,

    ['Dawon'] = function(mob)
        doMobSkillEveryHPP(mob, 20, 80, xi.mobSkill.PERFECT_DODGE_1, not mob:hasStatusEffect(xi.effect.PERFECT_DODGE))
    end
}

-----------------------------------
-- Mob On Init
-----------------------------------
xi.voidwalker.onMobInitialize = function(mob)
end

xi.voidwalker.onMobSpawn = function(mob)
    local mobName = mob:getName()
    mob:setStatus(xi.status.INVISIBLE)
    mob:hideHP(true)
    mob:hideName(true)
    mob:setUntargetable(true)
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
    local mixin   = mixinByMobName[mobName]

    if mixin then
        mixin(mob)
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
    mob:setLocalVar('[VoidWalker]PopedBy', 0)
    mob:setLocalVar('[VoidWalker]checkPopedBy', 0)
    mob:setLocalVar('[VoidWalker]PopedWith', 0)
    mob:setLocalVar('[VoidWalker]PopedAt', 0)
    mob:setLocalVar('MOBSKILL_USE', 0)
    mob:setLocalVar('MOBSKILL_TIME', 0)
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
    mob:setLocalVar('[VoidWalker]PopedBy', 0)
    mob:setLocalVar('[VoidWalker]checkPopedBy', 0)
    mob:setLocalVar('[VoidWalker]PopedWith', 0)
    mob:setLocalVar('[VoidWalker]PopedAt', 0)
    mob:setLocalVar('MOBSKILL_USE', 0)
    mob:setLocalVar('MOBSKILL_TIME', 0)
    DespawnPet(mob)
end

xi.voidwalker.onMobDeath = function(mob, player, optParams, keyItem)
    if player then
        local popkeyitem = mob:getLocalVar('[VoidWalker]PopedWith')

        if optParams.isKiller then
            local playerpoped = GetPlayerByID(mob:getLocalVar('[VoidWalker]PopedBy'))
            local alliance    = player:getAlliance()
            local popperInAlliance = false

            for _, member in pairs(alliance) do
                if
                    playerpoped and
                    member:getID() == playerpoped:getID()
                then
                    popperInAlliance = true
                    break
                end
            end

            if
                xi.voidwalker.shouldUpgradeOriginalPopper(
                    optParams.isKiller,
                    playerpoped ~= nil,
                    popperInAlliance,
                    playerpoped and playerpoped:hasKeyItem(keyItem)
                )
            then
                checkUpgrade(playerpoped, mob, keyItem)
            end
        end

        if
            xi.voidwalker.shouldUpgradeKiller(
                player:hasKeyItem(popkeyitem),
                player:hasKeyItem(keyItem)
            )
        then
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

    if
        #abyssites == 0 or
        not zones[zoneId].mob or
        not zones[zoneId].mob.VOIDWALKER
    then
        return
    end

    local mobs       = getMobsFromAbyssites(zoneId, abyssites)
    local mobNearest = getNearestMob(player, mobs)

    if not mobNearest then
        player:messageSpecial(zoneTextTable.VOIDWALKER_NO_MOB, abyssites[1])
    elseif xi.voidwalker.healingRangeOutcome(mobNearest.distance) == 'spawn' then
        local mob = GetMobByID(mobNearest.mobId)
        if not mob then
            return
        end

        mob:setLocalVar('[VoidWalker]PopedBy', player:getID())
        mob:setLocalVar('[VoidWalker]PopedWith', mobNearest.keyItem)
        mob:setLocalVar('[VoidWalker]PopedAt', GetSystemTime())

        if xi.voidwalker.shouldConsumePopAbyssite(mobNearest.keyItem) then
            player:delKeyItem(mobNearest.keyItem)
            player:messageSpecial(zoneTextTable.VOIDWALKER_BREAK_KI, mobNearest.keyItem)
        else
            player:messageSpecial(zoneTextTable.VOIDWALKER_SPAWN_MOB)
            mob:hideHP(false)
        end

        mob:hideName(false)
        mob:setUntargetable(false)
        mob:setStatus(xi.status.UPDATE)
        mob:updateClaim(player)

    elseif xi.voidwalker.healingRangeOutcome(mobNearest.distance) == 'too_far' then
        player:messageSpecial(zoneTextTable.VOIDWALKER_MOB_TOO_FAR, mobNearest.keyItem)

    else
        local mob       = GetMobByID(mobNearest.mobId)
        local direction = getDirection(player, mob, mobNearest.distance)
        player:messageSpecial(zoneTextTable.VOIDWALKER_MOB_HINT, abyssiteMessage[mobNearest.keyItem], direction, mobNearest.distance, mobNearest.keyItem)
    end
end
