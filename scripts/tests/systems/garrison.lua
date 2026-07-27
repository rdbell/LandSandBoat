require('scripts/globals/garrison_data')
require('scripts/globals/garrison')

local function playerWithPartyCount(numParties)
    local alliance = {}

    for i = 1, numParties do
        local name = tostring(i)
        local leader = { getName = function() return name end }
        alliance[i] = { getPartyLeader = function() return leader end }
    end

    return { getAlliance = function() return alliance end }
end

local function playerWithPartyLeaders(names)
    local alliance = {}

    for i, name in ipairs(names) do
        local leader = { getName = function() return name end }
        alliance[i] = { getPartyLeader = function() return leader end }
    end

    return { getAlliance = function() return alliance end }
end

describe('Garrison wave schedules', function()
    it('defines every party-size wave group and its delay', function()
        local schedules = xi.garrison.waves.spawnSchedule
        assert(schedules[1][1][1] == 2)
        assert(schedules[1][4][1] == 2 and schedules[1][4][4] == 2)
        assert(schedules[2][2][1] == 4 and schedules[2][2][2] == 2)
        assert(schedules[3][1][1] == 4 and schedules[3][1][2] == 2)
        assert(schedules[3][2][1] == 6 and schedules[3][2][2] == 2)
        assert(schedules[3][3][1] == 4 and schedules[3][3][3] == 2)
        assert(xi.garrison.waves.delayBetweenGroups == 15)
    end)
end)

describe('Garrison loot', function()
    it('defines the complete weighted catalog for every level cap', function()
        assert(#xi.garrison.loot[20] == 6 and #xi.garrison.loot[30] == 7)
        assert(#xi.garrison.loot[40] == 6 and #xi.garrison.loot[50] == 14 and #xi.garrison.loot[99] == 14)
        assert(xi.garrison.loot[20][1].itemId == xi.item.DRAGON_CHRONICLES and xi.garrison.loot[20][1].weight == 1000)
        assert(xi.garrison.loot[99][1].itemId == xi.item.MIRATETES_MEMOIRS and xi.garrison.loot[99][14].itemId == xi.item.MIGHTY_SWORD)
    end)
end)

describe('Garrison level-cap-20 layouts', function()
    it('defines the three starter-zone encounter layouts', function()
        local ronfaure = xi.garrison.zoneData[xi.zone.WEST_RONFAURE]
        assert(ronfaure.itemReq == xi.item.RED_CRYPTEX and ronfaure.levelCap == 20 and ronfaure.mobBoss == 'Orcish_Fighterchief')

        local gustaberg = xi.garrison.zoneData[xi.zone.NORTH_GUSTABERG]
        assert(gustaberg.itemReq == xi.item.DARKSTEEL_ENGRAVING and gustaberg.pos[1] == -575 and gustaberg.xChange == 1)

        local sarutabaruta = xi.garrison.zoneData[xi.zone.WEST_SARUTABARUTA]
        assert(sarutabaruta.itemReq == xi.item.SEVEN_KNOT_QUIPU and sarutabaruta.pos[2] == -13.312 and sarutabaruta.pos[4] == 128)
    end)
end)

describe('Garrison level-cap-30 layouts', function()
    it('defines all six intermediate-zone encounter layouts', function()
        assert(xi.garrison.zoneData[xi.zone.VALKURM_DUNES].mobBoss == 'Goblin_Swindler')
        assert(xi.garrison.zoneData[xi.zone.JUGNER_FOREST].pos[2] == 0.499)
        assert(xi.garrison.zoneData[xi.zone.PASHHOW_MARSHLANDS].itemReq == xi.item.SILVER_ENGRAVING)
        assert(xi.garrison.zoneData[xi.zone.BUBURIMU_PENINSULA].zThirdLine == -4)
        assert(xi.garrison.zoneData[xi.zone.MERIPHATAUD_MOUNTAINS].xChange == 2)
        assert(xi.garrison.zoneData[xi.zone.QUFIM_ISLAND].textRegion == 10)
    end)
end)

describe('Garrison level-cap-40 layouts', function()
    it('defines all three highland encounter layouts', function()
        assert(xi.garrison.zoneData[xi.zone.BEAUCEDINE_GLACIER].pos[2] == -59.900)
        assert(xi.garrison.zoneData[xi.zone.THE_SANCTUARY_OF_ZITAH].mobBoss == 'Goblin_Doyen')
        assert(xi.garrison.zoneData[xi.zone.YUHTUNGA_JUNGLE].itemReq == xi.item.SHEEP_LEATHER_MISSIVE)
    end)
end)

describe('Garrison level-cap-50 layouts', function()
    it('defines the three endgame encounter layouts', function()
        assert(xi.garrison.zoneData[xi.zone.XARCABARD].mobBoss == 'Demon_Aristocrat')
        assert(xi.garrison.zoneData[xi.zone.EASTERN_ALTEPA_DESERT].pos[1] == -245)
        assert(xi.garrison.zoneData[xi.zone.YHOATOR_JUNGLE].zThirdLine == -2)
    end)
end)

describe('Garrison level-cap-99 layout', function()
    it('defines the Cape Teriggan encounter layout', function()
        local teriggan = xi.garrison.zoneData[xi.zone.CAPE_TERIGGAN]
        assert(teriggan.itemReq == xi.item.BUNNY_FANG_SACK and teriggan.levelCap == 99)
        assert(teriggan.mobBoss == 'Goblin_Boss' and teriggan.pos[1] == -174)
    end)
end)

describe('Garrison spawn schedule selection', function()
    it('uses the alliance party count and falls back to one party', function()
        assert(xi.garrison.getSpawnSchedule(playerWithPartyCount(1)) == xi.garrison.waves.spawnSchedule[1])
        assert(xi.garrison.getSpawnSchedule(playerWithPartyCount(2)) == xi.garrison.waves.spawnSchedule[2])
        assert(xi.garrison.getSpawnSchedule(playerWithPartyCount(3)) == xi.garrison.waves.spawnSchedule[3])
        assert(xi.garrison.getSpawnSchedule(playerWithPartyCount(0)) == xi.garrison.waves.spawnSchedule[1])
        assert(xi.garrison.getSpawnSchedule(playerWithPartyCount(4)) == xi.garrison.waves.spawnSchedule[1])
        assert(xi.garrison.getSpawnSchedule(playerWithPartyLeaders({ 'same', 'same' })) == xi.garrison.waves.spawnSchedule[1])
    end)
end)

describe('Garrison level-cap resolution', function()
    it('uses the server maximum for the uncapped encounter', function()
        local captured = {}
        local entity = {
            addStatusEffect = function(_, effect, args)
                captured.effect = effect
                captured.args = args
            end,
        }

        xi.garrison.addLevelCap(entity, 50)
        assert(captured.effect == xi.effect.LEVEL_RESTRICTION and captured.args.power == 50)
        assert(captured.args.origin == entity)
        assert(captured.args.flag == xi.effectFlag.ON_ZONE + xi.effectFlag.CONFRONTATION)

        xi.garrison.addLevelCap(entity, 99)
        assert(captured.args.power == xi.settings.main.MAX_LEVEL)
        assert(captured.args.flag == xi.effectFlag.ON_ZONE + xi.effectFlag.CONFRONTATION)
    end)
end)

describe('Garrison ally information', function()
    it('assembles ally data from zone and nation catalogs', function()
        local ronfaure = xi.garrison.zoneData[xi.zone.WEST_RONFAURE]
        local ally = xi.garrison.getAllyInfo(xi.zone.WEST_RONFAURE, ronfaure, xi.nation.SANDORIA)

        assert(ally.name == 'Patrician' and ally.groupId == 1)
        assert(ally.minLevel == 15 and ally.maxLevel == 20)
        assert(ally.pos == ronfaure.pos and ally.looks == xi.garrison.allyLooks[20][xi.nation.SANDORIA])
        assert(xi.garrison.getAllyInfo(xi.zone.WEST_RONFAURE, ronfaure, xi.nation.BEASTMEN) == nil)
    end)
end)

describe('Garrison ally formation', function()
    it('places allies in three configured lines', function()
        local zone = { getID = function() return xi.zone.WEST_RONFAURE end }
        local allyInfo = {
            name = 'Patrician',
            looks = { 'look' },
            pos = xi.garrison.zoneData[xi.zone.WEST_RONFAURE].pos,
        }
        local npcs = xi.garrison.rollNPCs(zone, allyInfo, 13)

        assert(#npcs == 13 and npcs[1].name == 'Patrician' and npcs[1].look == 'look')
        assert(npcs[1].pos[1] == -438 and npcs[1].pos[3] == -223)
        assert(npcs[6].pos[1] == -438 and npcs[6].pos[3] == -233)
        assert(npcs[7].pos[1] == -440 and npcs[7].pos[3] == -223)
        assert(npcs[12].pos[1] == -440 and npcs[12].pos[3] == -233)
        assert(npcs[13].pos[1] == -442 and npcs[13].pos[3] == -223)
    end)
end)

describe('Garrison ally NPC spawning', function()
    it('builds the dynamic ally specification and tracks ally deaths', function()
        local zoneData = { deadNPCCount = 0 }
        local inserted, deathListener = nil, nil
        local mob = {
            getName = function() return 'Patrician' end,
            getID = function() return 0 end,
            getMainLvl = function() return 15 end,
            getHP = function() return 100 end,
            setSpawn = function() end,
            setMobMod = function() end,
            setRoamFlags = function() end,
            spawn = function() end,
            setBaseSpeed = function() end,
            setAllegiance = function() end,
            setMagicCastingEnabled = function() end,
            setMobAbilityEnabled = function() end,
            addListener = function(_, event, name, callback)
                assert(event == 'DEATH' and name == 'GARRISON_NPC_DEATH')
                deathListener = callback
            end,
        }
        local zone = {
            insertDynamicEntity = function(_, spec)
                inserted = spec
                return mob
            end,
        }

        assert(xi.garrison.spawnNPC(zone, zoneData, { 1, 2, 3, 4 }, 'Patrician', 1, 'look', 15, 20) == mob)
        assert(inserted.objtype == xi.objType.MOB and inserted.allegiance == xi.allegiance.PLAYER)
        assert(inserted.name == 'Patrician' and inserted.x == 1 and inserted.y == 2 and inserted.z == 3 and inserted.rotation == 4)
        assert(inserted.look == 'look' and inserted.groupId == 1 and inserted.minLevel == 15 and inserted.maxLevel == 20)
        assert(inserted.groupZoneId == xi.zone.GM_HOME and inserted.releaseIdOnDisappear and inserted.specialSpawnAnimation)
        deathListener(mob)
        assert(zoneData.deadNPCCount == 1)
    end)
end)

describe('Garrison ally spawn admission', function()
    it('requires a player-controlled region, ally data, and a nonempty formation', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = { players = { 1, 2 }, npcs = {}, levelCap = 20 }
        local nation, allyInfo, rolled = xi.nation.SANDORIA, { groupId = 1, minLevel = 15, maxLevel = 20 }, { { pos = { 1, 2, 3, 4 }, name = 'Patrician', look = 'look' } }
        local spawned = 0
        local zone = { getID = function() return zoneID end, getRegionID = function() return 0 end }
        local originalGetAllyInfo, originalRollNPCs, originalSpawnNPC = xi.garrison.getAllyInfo, xi.garrison.rollNPCs, xi.garrison.spawnNPC

        stub('GetRegionOwner', function() return nation end)
        xi.garrison.getAllyInfo = function() return allyInfo end
        xi.garrison.rollNPCs = function() return rolled end
        xi.garrison.spawnNPC = function()
            spawned = spawned + 1
            return { getID = function() return spawned end, addStatusEffect = function() end }
        end

        assert(xi.garrison.spawnNPCs(zone, zoneData))
        assert(spawned == 1 and #zoneData.npcs == 1 and zoneData.npcs[1] == 1)

        nation = xi.nation.BEASTMEN
        assert(not xi.garrison.spawnNPCs(zone, zoneData))
        nation = xi.nation.SANDORIA
        allyInfo = nil
        assert(not xi.garrison.spawnNPCs(zone, zoneData))
        allyInfo, rolled = { groupId = 1, minLevel = 15, maxLevel = 20 }, {}
        assert(not xi.garrison.spawnNPCs(zone, zoneData))

        xi.garrison.getAllyInfo, xi.garrison.rollNPCs, xi.garrison.spawnNPC = originalGetAllyInfo, originalRollNPCs, originalSpawnNPC
    end)
end)

describe('Garrison mob-pool selection', function()
    it('filters excluded IDs and samples without replacement', function()
        local selected = xi.garrison.pickMobsFromPool(10, 14, 3, { 11, 13 })
        local seen = {}
        for _, mobID in ipairs(selected) do
            assert(mobID == 10 or mobID == 12 or mobID == 14)
            assert(not seen[mobID])
            seen[mobID] = true
        end
        assert(#selected == 3)
        assert(#xi.garrison.pickMobsFromPool(10, 14, 8, { 11, 13 }) == 3)
        assert(#xi.garrison.pickMobsFromPool(10, 14, 0, {}) == 0)
    end)
end)

describe('Garrison mob spawning', function()
    it('configures spawned mobs and tracks both death and despawn', function()
        local zoneData = { levelCap = 30, mobs = {}, deadMobCount = 0, despawnedMobCount = 0 }
        local listeners = {}
        local mob = {
            addStatusEffect = function() end,
            setRoamFlags = function() end,
            addListener = function(_, event, name, callback) listeners[event] = { name, callback } end,
        }

        stub('SpawnMob', mob)
        assert(xi.garrison.spawnMob(123, zoneData) == mob)
        assert(#zoneData.mobs == 1 and zoneData.mobs[1] == 123)
        assert(listeners.DEATH[1] == 'GARRISON_MOB_DEATH' and listeners.DESPAWN[1] == 'GARRISON_MOB_DESPAWN')
        listeners.DEATH[2](mob)
        listeners.DESPAWN[2](mob)
        assert(zoneData.deadMobCount == 1 and zoneData.despawnedMobCount == 1)
    end)
end)

describe('Garrison Gil payout', function()
    it('awards each participating player an equal cap-scaled amount', function()
        local first = { gil = 0, messages = 0 }
        function first:addGil(gil) self.gil = self.gil + gil end
        function first:messageSpecial(_, gil) self.messages = self.messages + gil end
        function first:getZoneID() return xi.zone.WEST_RONFAURE end

        local second = { gil = 0, messages = 0 }
        function second:addGil(gil) self.gil = self.gil + gil end
        function second:messageSpecial(_, gil) self.messages = self.messages + gil end
        function second:getZoneID() return xi.zone.WEST_RONFAURE end

        xi.garrison.handleGilPayout(30, { first, second })
        local expected = xi.settings.main.GIL_RATE * 30 * 100
        assert(first.gil == expected and second.gil == expected)
        assert(first.messages == expected and second.messages == expected)
    end)
end)

describe('Garrison loot rolls', function()
    it('rolls once per player and deposits every reward into the first player pool', function()
        local originalSelect = utils.selectFromLootGroups
        local lootGroup = xi.garrison.loot[20]
        local originalQuantity = lootGroup.quantity
        local rewards = {}
        local first = {
            getName = function() return 'first' end,
            addTreasure = function(_, itemID) table.insert(rewards, itemID) end,
        }
        local second = { getName = function() return 'second' end }

        utils.selectFromLootGroups = function(actor, lootTable)
            assert(actor == first and lootTable[1] == lootGroup)
            assert(lootGroup.quantity == 2)
            return { { itemId = 111 }, { itemId = 222 } }
        end
        xi.garrison.handleLootRolls(20, { first, second })

        assert(#rewards == 2 and rewards[1] == 111 and rewards[2] == 222)

        utils.selectFromLootGroups = originalSelect
        lootGroup.quantity = originalQuantity
    end)
end)

describe('Garrison spawn-NPC state transition', function()
    it('enters battle only when ally spawning succeeds', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local originalState = zoneData.state
        local originalPlayers = zoneData.players
        local originalRunning = zoneData.isRunning
        local originalSpawnNPCs = xi.garrison.spawnNPCs
        local zone = { getID = function() return zoneID end }
        local npc = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
        }

        zoneData.players = {}
        zoneData.isRunning = false

        xi.garrison.spawnNPCs = function() return true end
        zoneData.state = xi.garrison.state.SPAWN_NPCS
        xi.garrison.tick(npc)
        assert(zoneData.state == xi.garrison.state.BATTLE)

        xi.garrison.spawnNPCs = function() return false end
        zoneData.state = xi.garrison.state.SPAWN_NPCS
        xi.garrison.tick(npc)
        assert(zoneData.state == xi.garrison.state.ENDED)

        xi.garrison.spawnNPCs = originalSpawnNPCs
        zoneData.state = originalState
        zoneData.players = originalPlayers
        zoneData.isRunning = originalRunning
    end)
end)

describe('Garrison tick scheduling', function()
    it('records its tick time and schedules another tick while running', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local originalState, originalPlayers = zoneData.state, zoneData.players
        local originalRunning, originalLastTick = zoneData.isRunning, zoneData.lastTick
        local originalSpawnNPCs = xi.garrison.spawnNPCs
        local scheduledDelay = nil
        local zone = { getID = function() return zoneID end }
        local npc = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
            timer = function(_, delay) scheduledDelay = delay end,
        }

        zoneData.players = {}
        zoneData.isRunning = true
        zoneData.state = xi.garrison.state.SPAWN_NPCS
        xi.garrison.spawnNPCs = function() return true end
        local before = GetSystemTime()
        xi.garrison.tick(npc)

        assert(zoneData.lastTick >= before and scheduledDelay == 1000)

        xi.garrison.spawnNPCs = originalSpawnNPCs
        zoneData.state, zoneData.players = originalState, originalPlayers
        zoneData.isRunning, zoneData.lastTick = originalRunning, originalLastTick
    end)
end)

describe('Garrison wave advance', function()
    it('resets mob state and schedules the next wave', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local original = {
            state = zoneData.state,
            players = zoneData.players,
            isRunning = zoneData.isRunning,
            spawnSchedule = zoneData.spawnSchedule,
            waveIndex = zoneData.waveIndex,
            groupIndex = zoneData.groupIndex,
            mobs = zoneData.mobs,
            deadMobCount = zoneData.deadMobCount,
            despawnedMobCount = zoneData.despawnedMobCount,
        }
        local zone = { getID = function() return zoneID end }
        local npc = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
        }

        zoneData.players = {}
        zoneData.isRunning = false
        zoneData.spawnSchedule = xi.garrison.waves.spawnSchedule[1]
        zoneData.state = xi.garrison.state.ADVANCE_WAVE
        zoneData.waveIndex = 2
        zoneData.groupIndex = 3
        zoneData.mobs = { 123, 456 }
        zoneData.deadMobCount = 2
        zoneData.despawnedMobCount = 2
        local before = GetSystemTime()
        xi.garrison.tick(npc)

        assert(zoneData.state == xi.garrison.state.BATTLE)
        assert(zoneData.waveIndex == 3 and zoneData.groupIndex == 1)
        assert(#zoneData.mobs == 0 and zoneData.deadMobCount == 0 and zoneData.despawnedMobCount == 0)
        assert(zoneData.nextSpawnTime >= before + xi.garrison.waves.delayBetweenGroups)

        for key, value in pairs(original) do
            zoneData[key] = value
        end
    end)
end)

describe('Garrison mob spawning', function()
    it('uses the current wave group and returns to battle', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local original = {
            state = zoneData.state,
            players = zoneData.players,
            isRunning = zoneData.isRunning,
            spawnSchedule = zoneData.spawnSchedule,
            waveIndex = zoneData.waveIndex,
            groupIndex = zoneData.groupIndex,
            mobs = zoneData.mobs,
            npcs = zoneData.npcs,
        }
        local spawned = {}
        local zone = {
            getID = function() return zoneID end,
            queryEntitiesByName = function()
                return { { getID = function() return 100 end } }
            end,
        }
        local npc = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
        }
        local originalSpawnMob = xi.garrison.spawnMob

        zoneData.players = {}
        zoneData.isRunning = false
        zoneData.spawnSchedule = { { 2, 1 } }
        zoneData.waveIndex = 1
        zoneData.groupIndex = 1
        zoneData.mobs = { 92 }
        zoneData.npcs = {}
        zoneData.state = xi.garrison.state.SPAWN_MOBS
        xi.garrison.spawnMob = function(mobID) table.insert(spawned, mobID) end
        xi.garrison.tick(npc)

        assert(#spawned == 2 and spawned[1] ~= 92 and spawned[2] ~= 92 and spawned[1] ~= spawned[2])
        assert(zoneData.state == xi.garrison.state.BATTLE and zoneData.groupIndex == 2)

        xi.garrison.spawnMob = originalSpawnMob
        for key, value in pairs(original) do
            zoneData[key] = value
        end
    end)
end)

describe('Garrison boss spawning', function()
    it('enters battle only after the boss spawns', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local original = { state = zoneData.state, players = zoneData.players, isRunning = zoneData.isRunning, npcs = zoneData.npcs, bossSpawned = zoneData.bossSpawned }
        local zone = { getID = function() return zoneID end, queryEntitiesByName = function() return { { getID = function() return 100 end } } end }
        local npc = { getZone = function() return zone end, getZoneID = function() return zoneID end }
        local originalSpawnMob = xi.garrison.spawnMob

        zoneData.players, zoneData.isRunning, zoneData.npcs = {}, false, {}
        xi.garrison.spawnMob = function() return {} end
        zoneData.state, zoneData.bossSpawned = xi.garrison.state.SPAWN_BOSS, false
        xi.garrison.tick(npc)
        assert(zoneData.state == xi.garrison.state.BATTLE and zoneData.bossSpawned)

        xi.garrison.spawnMob = function() return nil end
        zoneData.state, zoneData.bossSpawned = xi.garrison.state.SPAWN_BOSS, false
        xi.garrison.tick(npc)
        assert(zoneData.state == xi.garrison.state.ENDED and not zoneData.bossSpawned)

        xi.garrison.spawnMob = originalSpawnMob
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison mob aggro', function()
    it('assigns reciprocal enmity between each spawned mob and ally', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local original = { state = zoneData.state, players = zoneData.players, isRunning = zoneData.isRunning, spawnSchedule = zoneData.spawnSchedule, waveIndex = zoneData.waveIndex, groupIndex = zoneData.groupIndex, mobs = zoneData.mobs, npcs = zoneData.npcs }
        local calls = {}
        local mob = { addEnmity = function(_, target) table.insert(calls, target) end }
        local ally = { addEnmity = function(_, target) table.insert(calls, target) end }
        local zone = { getID = function() return zoneID end, queryEntitiesByName = function() return { { getID = function() return 100 end } } end }
        local npc = { getZone = function() return zone end, getZoneID = function() return zoneID end }
        local originalSpawnMob = xi.garrison.spawnMob

        stub('GetMobByID', function(id)
            if id == 92 then return mob end
            if id == 200 then return ally end
        end)
        zoneData.players, zoneData.isRunning = {}, false
        zoneData.spawnSchedule, zoneData.waveIndex, zoneData.groupIndex = { { 1 } }, 1, 1
        zoneData.mobs, zoneData.npcs, zoneData.state = {}, { 200 }, xi.garrison.state.SPAWN_MOBS
        xi.garrison.spawnMob = function() end
        xi.garrison.tick(npc)

        assert(#calls == 2 and calls[1] == ally and calls[2] == mob)

        xi.garrison.spawnMob = originalSpawnMob
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison reward transition', function()
    it('grants loot and Gil before ending the event', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local original = { state = zoneData.state, players = zoneData.players, isRunning = zoneData.isRunning, levelCap = zoneData.levelCap }
        local zone = { getID = function() return zoneID end }
        local npc = { getZone = function() return zone end, getZoneID = function() return zoneID end }
        local lootCalls, gilCalls = 0, 0
        local originalLoot, originalGil = xi.garrison.handleLootRolls, xi.garrison.handleGilPayout
        zoneData.players, zoneData.isRunning, zoneData.levelCap = {}, false, 30
        xi.garrison.handleLootRolls = function(cap) assert(cap == 30) lootCalls = lootCalls + 1 end
        xi.garrison.handleGilPayout = function(cap) assert(cap == 30) gilCalls = gilCalls + 1 end
        zoneData.state = xi.garrison.state.GRANT_LOOT
        xi.garrison.tick(npc)
        assert(lootCalls == 1 and gilCalls == 1 and zoneData.state == xi.garrison.state.ENDED)
        xi.garrison.handleLootRolls, xi.garrison.handleGilPayout = originalLoot, originalGil
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison entry validation', function()
    it('accepts eligible players and rejects active or wrong-nation entries', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local originalRunning = zoneData.isRunning
        local player = {
            getZoneID = function() return zoneID end,
            getAlliance = function() return {} end,
            getNation = function() return xi.nation.SANDORIA end,
            getRank = function() return xi.settings.main.GARRISON_RANK end,
            getCharVar = function() return 0 end,
            getZone = function() return { getID = function() return zoneID end } end,
            messageText = function() end,
            messageSpecial = function() end,
        }

        zoneData.isRunning = false
        assert(xi.garrison.validateEntry(zoneData, player, {}, xi.nation.SANDORIA))
        assert(not xi.garrison.validateEntry(zoneData, player, {}, xi.nation.BASTOK))
        zoneData.isRunning = true
        assert(not xi.garrison.validateEntry(zoneData, player, {}, xi.nation.SANDORIA))
        zoneData.isRunning = originalRunning
    end)
end)

describe('Garrison trade admission', function()
    it('starts the nation event only after all trade gates pass', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local originalAllyInfo = xi.garrison.getAllyInfo
        local originalValidate = xi.garrison.validateEntry
        local originalTradeCheck = npcUtil.tradeHasExactly
        local started, storedNPC = nil, nil
        local zone = { getID = function() return zoneID end, getRegionID = function() return 0 end }
        local player = {
            getZone = function() return zone end,
            getNation = function() return xi.nation.SANDORIA end,
            startEvent = function(_, event) started = event end,
            setLocalVar = function(_, key, value) storedNPC = { key, value } end,
        }
        local npc = { getID = function() return 1234 end }

        xi.garrison.getAllyInfo = function() return {} end
        xi.garrison.validateEntry = function() return true end
        npcUtil.tradeHasExactly = function() return true end
        assert(xi.garrison.onTrade(player, npc, {}, xi.nation.SANDORIA))
        assert(started == 32753 and storedNPC[1] == 'GARRISON_NPC' and storedNPC[2] == 1234)

        xi.garrison.validateEntry = function() return false end
        assert(not xi.garrison.onTrade(player, npc, {}, xi.nation.SANDORIA))

        xi.garrison.getAllyInfo = originalAllyInfo
        xi.garrison.validateEntry = originalValidate
        npcUtil.tradeHasExactly = originalTradeCheck
    end)
end)

describe('Garrison event confirmation', function()
    it('confirms a matching event and starts Garrison', function()
        local originalStart = xi.garrison.start
        local originalEnabled = xi.settings.main.ENABLE_GARRISON
        local confirmed, started = false, false
        local player = {
            getNation = function() return xi.nation.SANDORIA end,
            getLocalVar = function() return 1234 end,
            confirmTrade = function() confirmed = true end,
        }
        xi.garrison.start = function() started = true end

        assert(xi.garrison.onEventFinish(player, 32753, 0))
        assert(confirmed and started)
        assert(not xi.garrison.onEventFinish(player, 32754, 0))
        assert(not xi.garrison.onEventFinish(player, 32753, 1))

        xi.settings.main.ENABLE_GARRISON = false
        assert(not xi.garrison.onEventFinish(player, 32753, 0))

        xi.garrison.start = originalStart
        xi.settings.main.ENABLE_GARRISON = originalEnabled
    end)
end)

describe('Garrison startup', function()
    it('initializes the first wave and timing state', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local keys = { 'players', 'spawnSchedule', 'npcs', 'mobs', 'state', 'isRunning', 'stateTime', 'waveIndex', 'groupIndex', 'bossSpawned', 'nextSpawnTime', 'endTime', 'deadNPCCount', 'deadMobCount', 'despawnedMobCount', 'lastTick' }
        local original = {}
        for _, key in ipairs(keys) do original[key] = zoneData[key] end
        local originalTick = xi.garrison.tick
        local zone = { getID = function() return zoneID end }
        local lockoutKey = '[Garrison]NextEntryTime_' .. zoneID
        local originalZoneLockout = GetServerVariable(lockoutKey)
        local tallyLockout = {}
        local player = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
            getAlliance = function() return {} end,
            setCharVar = function(_, key, value, expires) tallyLockout = { key, value, expires } end,
        }
        local npc = { timer = function() end }
        xi.garrison.tick = function() end
        local before = GetSystemTime()
        xi.garrison.start(player, npc)
        assert(zoneData.state == xi.garrison.state.SPAWN_NPCS and zoneData.isRunning)
        assert(zoneData.waveIndex == 1 and zoneData.groupIndex == 1 and not zoneData.bossSpawned)
        assert(#zoneData.players == 0 and #zoneData.npcs == 0 and #zoneData.mobs == 0)
        assert(zoneData.nextSpawnTime >= before + xi.garrison.waves.delayBetweenGroups)
        assert(zoneData.endTime >= before + xi.settings.main.GARRISON_TIME_LIMIT)
        assert(tallyLockout[1] == '[Garrison]NextEntryTime' and tallyLockout[2] == 1 and tallyLockout[3] == NextConquestTally())
        assert(GetServerVariable(lockoutKey) >= before + xi.settings.main.GARRISON_LOCKOUT)
        xi.garrison.tick = originalTick
        SetServerVariable(lockoutKey, originalZoneLockout)
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison shutdown', function()
    it('clears active collections and running state', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local keys = { 'players', 'spawnSchedule', 'npcs', 'mobs', 'isRunning' }
        local original = {}
        for _, key in ipairs(keys) do original[key] = zoneData[key] end
        local zone = { getID = function() return zoneID end }
        zoneData.players = {}
        zoneData.spawnSchedule = { { 2 } }
        zoneData.npcs = {}
        zoneData.mobs = {}
        zoneData.isRunning = true
        xi.garrison.stop(zone)
        assert(#zoneData.players == 0 and #zoneData.spawnSchedule == 0)
        assert(#zoneData.npcs == 0 and #zoneData.mobs == 0 and not zoneData.isRunning)
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison shutdown actions', function()
    it('clears membership before removing caps and despawns all active mobs', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local keys = { 'players', 'spawnSchedule', 'npcs', 'mobs', 'isRunning' }
        local original = {}
        for _, key in ipairs(keys) do original[key] = zoneData[key] end
        local removedCaps, despawned, clearedZone = {}, {}, nil
        local players = {
            [11] = { delStatusEffect = function() table.insert(removedCaps, 11) end },
            [12] = { delStatusEffect = function() table.insert(removedCaps, 12) end },
        }
        local zone = { getID = function() return zoneID end }

        stub('GetPlayerByID', function(id) return players[id] end)
        stub('DespawnMob', function(id, passedZone)
            assert(passedZone == zone)
            table.insert(despawned, id)
        end)
        stub('ClearGarrisonZonePlayers', function(id) clearedZone = id end)
        zoneData.players, zoneData.spawnSchedule = { 11, 12 }, { { 2 } }
        zoneData.npcs, zoneData.mobs, zoneData.isRunning = { 21 }, { 31, 32 }, true
        xi.garrison.stop(zone)

        assert(clearedZone == zoneID and #removedCaps == 2)
        assert(#despawned == 3 and despawned[1] == 21 and despawned[2] == 31 and despawned[3] == 32)
        assert(#zoneData.players == 0 and #zoneData.spawnSchedule == 0 and #zoneData.npcs == 0 and #zoneData.mobs == 0 and not zoneData.isRunning)

        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison watchdog', function()
    it('stops a running event whose main tick is more than two seconds late', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local keys = { 'players', 'spawnSchedule', 'npcs', 'mobs', 'state', 'isRunning', 'stateTime', 'waveIndex', 'groupIndex', 'bossSpawned', 'nextSpawnTime', 'endTime', 'deadNPCCount', 'deadMobCount', 'despawnedMobCount', 'lastTick' }
        local original = {}
        for _, key in ipairs(keys) do original[key] = zoneData[key] end
        local originalTick, originalStop = xi.garrison.tick, xi.garrison.stop
        local callbacks = {}
        local zone = { getID = function() return zoneID end, getName = function() return 'West Ronfaure' end }
        local player = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
            getAlliance = function() return {} end,
            setCharVar = function() end,
        }
        local npc = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
            timer = function(_, delay, callback) callbacks[delay] = callback end,
        }
        local stopped = false

        xi.garrison.tick = function() end
        xi.garrison.stop = function(stoppedZone) stopped = stoppedZone == zone end
        xi.garrison.start(player, npc)
        zoneData.lastTick = GetSystemTime() - 3
        callbacks[5000](npc)

        assert(stopped)

        xi.garrison.tick, xi.garrison.stop = originalTick, originalStop
        for key, value in pairs(original) do zoneData[key] = value end
    end)

    it('re-arms after a timely main tick', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local keys = { 'players', 'spawnSchedule', 'npcs', 'mobs', 'state', 'isRunning', 'stateTime', 'waveIndex', 'groupIndex', 'bossSpawned', 'nextSpawnTime', 'endTime', 'deadNPCCount', 'deadMobCount', 'despawnedMobCount', 'lastTick' }
        local original = {}
        for _, key in ipairs(keys) do original[key] = zoneData[key] end
        local originalTick, originalStop = xi.garrison.tick, xi.garrison.stop
        local callback, timerCalls, stopped = nil, 0, false
        local zone = { getID = function() return zoneID end, getName = function() return 'West Ronfaure' end }
        local player = { getZone = function() return zone end, getZoneID = function() return zoneID end, getAlliance = function() return {} end, setCharVar = function() end }
        local npc = {
            getZone = function() return zone end,
            getZoneID = function() return zoneID end,
            timer = function(_, delay, fn)
                assert(delay == 5000)
                timerCalls, callback = timerCalls + 1, fn
            end,
        }

        xi.garrison.tick = function() end
        xi.garrison.stop = function() stopped = true end
        xi.garrison.start(player, npc)
        zoneData.lastTick = GetSystemTime()
        callback(npc)

        assert(not stopped and zoneData.isRunning and timerCalls == 2)

        xi.garrison.tick, xi.garrison.stop = originalTick, originalStop
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)

describe('Garrison win transition', function()
    it('moves the zone into the reward state', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local originalState = zoneData.state
        xi.garrison.win({ getID = function() return zoneID end })
        assert(zoneData.state == xi.garrison.state.GRANT_LOOT)
        zoneData.state = originalState
    end)
end)

describe('Garrison battle transition', function()
    it('moves to mob spawning when the next group is due', function()
        local zoneID = xi.zone.WEST_RONFAURE
        local zoneData = xi.garrison.zoneData[zoneID]
        local keys = { 'state', 'players', 'npcs', 'mobs', 'deadNPCCount', 'deadMobCount', 'despawnedMobCount', 'spawnSchedule', 'waveIndex', 'groupIndex', 'nextSpawnTime', 'endTime', 'bossSpawned', 'isRunning' }
        local original = {}
        for _, key in ipairs(keys) do original[key] = zoneData[key] end
        local player = xi.test.world:spawnPlayer({ zone = zoneID })
        local zone = { getID = function() return zoneID end }
        local npc = { getZone = function() return zone end, getZoneID = function() return zoneID end }
        local now = GetSystemTime()
        zoneData.state = xi.garrison.state.BATTLE
        zoneData.players, zoneData.npcs, zoneData.mobs = { player:getID() }, { 1 }, {}
        zoneData.deadNPCCount, zoneData.deadMobCount, zoneData.despawnedMobCount = 0, 0, 0
        zoneData.spawnSchedule, zoneData.waveIndex, zoneData.groupIndex = { { 2 } }, 1, 1
        zoneData.nextSpawnTime, zoneData.endTime, zoneData.bossSpawned, zoneData.isRunning = now - 1, now + 60, false, false
        xi.garrison.tick(npc)
        assert(zoneData.state == xi.garrison.state.SPAWN_MOBS)
        player:release()
        for key, value in pairs(original) do zoneData[key] = value end
    end)
end)
