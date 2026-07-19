require('scripts/globals/garrison_data')

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
