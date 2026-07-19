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
