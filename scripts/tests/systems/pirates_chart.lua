require('scripts/globals/pirates_chart')

describe("Pirates Chart loot catalog", function()
    it('pins all four weighted loot groups', function()
        local loot = xi.piratesChart.loot
        assert(#loot == 4 and #loot[1] == 7 and #loot[2] == 7 and #loot[3] == 3 and #loot[4] == 1)
        assert(loot[1][1].itemId == xi.item.CORAL_FRAGMENT and loot[1][1].weight == xi.loot.weight.VERY_LOW)
        assert(loot[1][6].itemId == xi.item.SHALL_SHELL and loot[1][6].weight == xi.loot.weight.VERY_HIGH)
        assert(loot[3][2].itemId == xi.item.MERCURIAL_KRIS and loot[3][2].weight == xi.loot.weight.EXTREMELY_LOW)
        assert(loot[4][1].itemId == xi.item.ALBATROSS_RING and loot[4][1].weight == 1000)
    end)
end)

describe("Pirates Chart trade", function()
    it('starts the chart event and prioritizes the party-size rejection', function()
        local message, event = nil, nil
        local player = {
            getParty = function() return {} end,
            getPartySize = function() return 3 end,
            checkSoloPartyAlliance = function() return 0 end,
            messageSpecial = function(_, ...) message = { ... } end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = { getStatus = function() return xi.status.NORMAL end }
        local box = { getStatus = function() return xi.status.DISAPPEAR end }
        local trade = {
            getSlotCount = function() return 1 end,
            getItemId = function() return xi.item.PIRATES_CHART end,
            getItemQty = function() return 1 end,
            confirmItem = function() end,
        }
        stub('GetNPCByID', function() return box end)
        xi.piratesChart.onTrade(player, npc, trade)
        assert(message[1] == 7825 and message[2] == xi.item.PIRATES_CHART)
        assert(event[1] == 14 and event[5] == 3)

        player.getPartySize = function() return 4 end
        player.checkSoloPartyAlliance = function() return 2 end
        message, event = nil, nil
        xi.piratesChart.onTrade(player, npc, trade)
        assert(message[1] == 7845 and message[2] == 3 and event == nil)

        player.getPartySize = function() return 3 end
        box.getStatus = function() return xi.status.NORMAL end
        message, event = nil, nil
        xi.piratesChart.onTrade(player, npc, trade)
        assert(message == nil and event == nil)
    end)
end)

describe("Pirates Chart event update", function()
    it('prepares every admitted party member for confrontation', function()
        local calls = { music = {} }
        local player = {
            getID = function() return 123 end,
            getParty = function() return {} end,
            confirmTrade = function() calls.confirmed = true end,
            setLocalVar = function(_, name, value) calls.active = { name, value } end,
            changeMusic = function(_, channel, song) calls.music[channel] = song end,
            delStatusEffectsByFlag = function(_, flag) calls.dispelable = flag end,
            delStatusEffect = function(_, effect) calls.reraise = effect end,
            delContainerItems = function(_, container) calls.temp = container end,
            addStatusEffect = function(_, effect, args) calls.effect, calls.args = effect, args end,
        }
        player.getParty = function() return { player } end
        local npc = { setLocalVar = function(_, name, value) calls.npcVar = { name, value } end }
        local box = { setLocalVar = function(_, name, value) calls.boxVar = { name, value } end }
        stub('GetNPCByID', function() return box end)
        xi.piratesChart.onEventUpdate(player, 14, 0, npc)
        assert(calls.confirmed and calls.active[1] == 'pChartActive' and calls.active[2] == 1)
        assert(calls.npcVar[1] == 'pChartMemberID_1' and calls.npcVar[2] == 123 and calls.boxVar[2] == 123)
        assert(calls.music[0] == 136 and calls.music[1] == 136 and calls.music[2] == 136 and calls.music[3] == 136)
        assert(calls.dispelable == xi.effectFlag.DISPELABLE and calls.reraise == xi.effect.RERAISE and calls.temp == xi.inv.TEMPITEMS)
        assert(calls.effect == xi.effect.LEVEL_RESTRICTION and calls.args.power == 20 and calls.args.origin == player)
    end)
end)

describe("Pirates Chart event finish", function()
    it('sets up the Taru and shimmering point only when both resolve', function()
        local calls = {}
        local player = { getParty = function() return {} end }
        local npc = { getLocalVar = function() return 0 end, setStatus = function(_, v) calls.qm = v end, timer = function(_, _, fn) calls.range = fn end }
        local taru = { setStatus = function(_, v) calls.taruStatus = v end, setAnimation = function(_, v) calls.taruAnimation = v end }
        local shimmering = { setStatus = function(_, v) calls.shimmerStatus = v end, timer = function(_, delay, fn) calls.delay, calls.shimmer = delay, fn end, entityAnimationPacket = function(_, v) calls.animation = v end }
        stub('GetNPCByID', function(id)
            if id == zones[xi.zone.VALKURM_DUNES].npc.PIRATE_CHART_TARU then return taru end
            if id == zones[xi.zone.VALKURM_DUNES].npc.SHIMMERING_POINT then return shimmering end
        end)
        stub('GetSystemTime', function() return 0 end)
        xi.piratesChart.onEventFinish(player, 14, 0, npc)
        -- rangeChecking immediately rejects the intentionally invalid player
        -- fixture and resets entities, so assert the delayed shimmer setup.
        assert(calls.taruAnimation == xi.animation.NONE)
        assert(calls.delay == 2000)
        calls.shimmer(shimmering)
        assert(calls.animation == xi.animationString.SHIMMER)
    end)
end)

describe("Pirates Chart range warning", function()
    it('warns immediately when the valid spawner first leaves range', function()
        local warned, delay = false, nil
        local restriction = { getPower = function() return 20 end }
        local player = {
            hasStatusEffect = function() return true end,
            getStatusEffect = function() return restriction end,
            getPartySize = function() return 1 end,
            checkSoloPartyAlliance = function() return 0 end,
            getZoneID = function() return xi.zone.VALKURM_DUNES end,
            checkDistance = function() return 11 end,
            messageSpecial = function() warned = true end,
        }
        local npc = {
            getLocalVar = function(_, name) return name == 'pChartSpawnerID' and 1 or 0 end,
            setStatus = function() end,
            timer = function(_, value) delay = value end,
        }
        local taru = { setStatus = function() end, setAnimation = function() end }
        local shimmering = { setStatus = function() end, timer = function() end }
        stub('GetPlayerByID', function(id) return id == 1 and player or nil end)
        stub('GetNPCByID', function(id)
            if id == zones[xi.zone.VALKURM_DUNES].npc.PIRATE_CHART_TARU then return taru end
            if id == zones[xi.zone.VALKURM_DUNES].npc.SHIMMERING_POINT then return shimmering end
        end)
        stub('GetSystemTime', function() return 100 end)

        xi.piratesChart.onEventFinish(player, 14, 0, npc)
        assert(warned and delay == 1000)
    end)
end)

describe("Pirates Chart event validity", function()
    it('rejects each invalid spawner and restricted member state', function()
        local current
        local rangeScheduled = false
        local inertNPC = {
            resetLocalVars = function() end,
            setStatus = function() end,
            setAnimation = function() end,
            timer = function() end,
        }
        local npc = {
            getLocalVar = function(_, name)
                if name == 'pChartSpawnerID' then return 1 end
                if name == 'pChartMemberID_1' and current.member then return 2 end
                return 0
            end,
            setStatus = function() end,
            timer = function() rangeScheduled = true end,
        }
        local triggeringPlayer = {
            getParty = function() return {} end,
            checkDistance = function() return 0 end,
        }
        local function player(config, id)
            local restriction = { getPower = function() return config.power end }
            return {
                getID = function() return id end,
                hasStatusEffect = function() return config.restricted end,
                getStatusEffect = function() return restriction end,
                getPartySize = function() return config.partySize end,
                checkSoloPartyAlliance = function() return config.alliance end,
                getZoneID = function() return config.zone end,
                checkDistance = function() return 0 end,
                getParty = function() return {} end,
            }
        end
        stub('GetPlayerByID', function(id)
            if id == 1 and current.spawner then return player(current.spawner, 1) end
            if id == 2 and current.member then return player(current.member, 2) end
        end)
        stub('GetNPCByID', function() return inertNPC end)
        stub('GetSystemTime', function() return 0 end)

        local valid = { restricted = true, power = 20, partySize = 3, alliance = 0, zone = xi.zone.VALKURM_DUNES }
        for _, testCase in ipairs({
            { name = 'valid', spawner = valid, expectRange = true },
            { name = 'missing spawner', expectRange = false },
            { name = 'missing restriction', spawner = { restricted = false, power = 20, partySize = 3, alliance = 0, zone = xi.zone.VALKURM_DUNES }, expectRange = false },
            { name = 'wrong restriction power', spawner = { restricted = true, power = 19, partySize = 3, alliance = 0, zone = xi.zone.VALKURM_DUNES }, expectRange = false },
            { name = 'oversized party', spawner = { restricted = true, power = 20, partySize = 4, alliance = 0, zone = xi.zone.VALKURM_DUNES }, expectRange = false },
            { name = 'alliance', spawner = { restricted = true, power = 20, partySize = 3, alliance = 2, zone = xi.zone.VALKURM_DUNES }, expectRange = false },
            { name = 'wrong zone', spawner = { restricted = true, power = 20, partySize = 3, alliance = 0, zone = 0 }, expectRange = false },
            { name = 'misconfigured member', spawner = valid, member = { restricted = true, power = 19, partySize = 1, alliance = 0, zone = xi.zone.VALKURM_DUNES }, expectRange = false },
            { name = 'member outside Valkurm', spawner = valid, member = { restricted = true, power = 19, partySize = 1, alliance = 0, zone = 0 }, expectRange = true },
            { name = 'member without restriction', spawner = valid, member = { restricted = false, power = 19, partySize = 1, alliance = 0, zone = xi.zone.VALKURM_DUNES }, expectRange = true },
        }) do
            current = testCase
            rangeScheduled = false
            xi.piratesChart.onEventFinish(triggeringPlayer, 14, 0, npc)
            assert(rangeScheduled == testCase.expectRange, testCase.name)
        end
    end)
end)

describe("Pirates Chart mob fight", function()
    it('uses Hundred Fists once below half health and restores damage after snare expiry', function()
        local calls = {}
        local used, now = 0, 11
        local mob = {
            getHPP = function() return 49 end,
            getLocalVar = function(_, name) return name == 'usedTwoHour' and used or 10 end,
            useMobAbility = function(_, skill) calls.skill = skill end,
            setLocalVar = function(_, name, value) if name == 'usedTwoHour' then used = value end end,
            setMobMod = function(_, _, value) calls.multiplier = value end,
        }
        stub('GetSystemTime', function() return now end)
        xi.piratesChart.onMobFight(mob)
        assert(calls.skill == xi.mobSkill.HUNDRED_FISTS_1 and used == 1 and calls.multiplier == 100)
        calls.skill = nil
        now = 10
        xi.piratesChart.onMobFight(mob)
        assert(calls.skill == nil)
    end)
end)

describe("Pirates Chart mob spawn", function()
    it('applies the fixed confrontation mob-mod policy', function()
        local mods = {}
        local mob = { setMobMod = function(_, id, value) mods[id] = value end }
        xi.piratesChart.onMobSpawn(mob)
        assert(mods[xi.mobMod.EXP_BONUS] == -100 and mods[xi.mobMod.NO_DROPS] == 1)
        assert(mods[xi.mobMod.GIL_MAX] == -1 and mods[xi.mobMod.IDLE_DESPAWN] == 60)
        assert(mods[xi.mobMod.BASE_DAMAGE_MULTIPLIER] == 100)
    end)
end)

describe("Pirates Chart buddy defeat gate", function()
    it('requires every other barnacle buddy to be defeated', function()
        local mobs = zones[xi.zone.VALKURM_DUNES].mob
        local alive = { [mobs.BEACH_MONK] = true }
        local mob = { getID = function() return mobs.BEACH_MONK end }
        stub('GetMobByID', function(id) return { isAlive = function() return alive[id] or false end } end)
        assert(xi.piratesChart.myBuddiesAreDead(mob))
        alive[mobs.HEIKE_CRAB] = true
        assert(not xi.piratesChart.myBuddiesAreDead(mob))
    end)
end)

describe("Pirates Chart item check", function()
    it('allows items only on the three Barnacle Buddy targets', function()
        local mobs = zones[xi.zone.VALKURM_DUNES].mob
        local target = { getID = function(self) return self.id end }

        for _, id in ipairs({ mobs.BEACH_MONK, mobs.HEIKE_CRAB, mobs.HOUU_THE_SHOALWADER }) do
            target.id = id
            assert(xi.piratesChart.onItemCheck(target) == 0)
        end

        target.id = 0
        assert(xi.piratesChart.onItemCheck(target) == xi.msg.basic.CANNOT_ON_THAT_TARG)
    end)
end)

describe("Pirates Chart box trigger", function()
    it('admits only the recorded chart spawner', function()
        local partyRequested = false
        local playerID = 9
        local player = {
            getID = function() return playerID end,
            getParty = function() partyRequested = true end,
        }
        local npc = { getLocalVar = function() return 10 end }
        stub('GetNPCByID', function() return nil end)

        xi.piratesChart.barnacledBoxOnTrigger(player, npc)
        assert(not partyRequested)

        playerID = 10
        xi.piratesChart.barnacledBoxOnTrigger(player, npc)
        assert(partyRequested)
    end)
end)

describe("Pirates Chart box rewards", function()
    it('opens once, distributes loot, and schedules disappearance', function()
        local calls = { timers = {} }
        local opened = 0
        local player = {
            getID = function() return 1 end,
            getParty = function() end,
            addTreasure = function(_, item) calls.reward = item end,
        }
        local npc = {
            getLocalVar = function(_, name) return name == 'pChartSpawnerID' and 1 or opened end,
            setLocalVar = function(_, _, value) opened = value end,
            entityAnimationPacket = function(_, animation) calls.animation = animation end,
            timer = function(_, delay) table.insert(calls.timers, delay) end,
        }
        local originalSelect = utils.selectFromLootGroups
        utils.selectFromLootGroups = function() return { { itemId = 42 } } end
        stub('GetNPCByID', function() return nil end)

        xi.piratesChart.barnacledBoxOnTrigger(player, npc)
        assert(calls.animation == xi.animationString.OPEN_CRATE_GLOW and calls.reward == 42)
        assert(opened == 1 and calls.timers[1] == 15000 and calls.timers[2] == 16000)

        xi.piratesChart.barnacledBoxOnTrigger(player, npc)
        assert(#calls.timers == 2)
        utils.selectFromLootGroups = originalSelect
    end)
end)

describe("Pirates Chart box spawn", function()
    it('spawns and arms the Barnacled Box after the final buddy dies', function()
        local calls = {}
        local mob = {
            getLocalVar = function() return 0 end,
            getPos = function() return 1, 2, 3 end,
            getRotPos = function() return 4 end,
            setLocalVar = function(_, name, value) calls.spawned = { name, value } end,
        }
        local box = {
            teleport = function(_, ...) calls.position = { ... } end,
            setStatus = function(_, value) calls.status = value end,
            setLocalVar = function(_, name, value) calls.open = { name, value } end,
            timer = function(_, delay) calls.delay = delay end,
        }
        local original = xi.piratesChart.myBuddiesAreDead
        xi.piratesChart.myBuddiesAreDead = function() return true end
        stub('GetNPCByID', function() return box end)
        xi.piratesChart.onMobDeath(mob)
        assert(calls.position[1] == 1 and calls.position[2] == 4 and calls.status == xi.status.NORMAL)
        assert(calls.open[1] == 'open' and calls.open[2] == 0 and calls.spawned[1] == 'spawnedChest' and calls.spawned[2] == 1)
        assert(calls.delay == 180000)
        xi.piratesChart.myBuddiesAreDead = original
    end)

    it('does nothing for duplicate, incomplete, or missing-box states', function()
        local calls = 0
        local current
        local mob = {
            getLocalVar = function() return current.spawned and 1 or 0 end,
            setLocalVar = function() calls = calls + 1 end,
        }
        local box = {
            teleport = function() calls = calls + 1 end,
            setStatus = function() calls = calls + 1 end,
            setLocalVar = function() calls = calls + 1 end,
            timer = function() calls = calls + 1 end,
        }
        local original = xi.piratesChart.myBuddiesAreDead
        stub('GetNPCByID', function() return current.box and box or nil end)

        for _, testCase in ipairs({
            { spawned = true, defeated = true, box = true },
            { spawned = false, defeated = false, box = true },
            { spawned = false, defeated = true, box = false },
        }) do
            current = testCase
            xi.piratesChart.myBuddiesAreDead = function() return current.defeated end
            xi.piratesChart.onMobDeath(mob)
            assert(calls == 0)
        end

        xi.piratesChart.myBuddiesAreDead = original
    end)
end)
