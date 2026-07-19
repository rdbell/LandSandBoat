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
