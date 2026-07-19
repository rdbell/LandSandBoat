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
