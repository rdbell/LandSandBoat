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
