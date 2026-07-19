require('scripts/globals/brigands_chart')

describe("Brigand's Chart trade", function()
    it('admits an exact chart trade at an unclaimed normal question mark', function()
        local message = nil
        local event = nil
        local confirmedItem = nil
        local confirmedQuantity = nil
        local player = {
            messageSpecial = function(_, ...) message = { ... } end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = {
            getStatus = function() return xi.status.NORMAL end,
            getLocalVar = function(_, name) return name == 'bChartSpawnerID' and 0 or 0 end,
        }
        local trade = {
            getSlotCount = function() return 1 end,
            getItemId = function() return xi.item.BRIGANDS_CHART end,
            getItemQty = function(_, item) return item == xi.item.BRIGANDS_CHART and 1 or 0 end,
            confirmItem = function(_, item, quantity)
                confirmedItem = item
                confirmedQuantity = quantity
            end,
        }

        xi.brigandsChart.onTrade(player, npc, trade)

        assert(confirmedItem == xi.item.BRIGANDS_CHART and confirmedQuantity == 1)
        assert(message[1] == 7850 and message[2] == xi.item.BRIGANDS_CHART)
        assert(event[1] == 902)
    end)
end)

describe("Brigand's Chart event update", function()
    it('activates the chart event after the trade confirmation', function()
        local calls = { music = {} }
        local player = {
            getID = function() return 123 end,
            confirmTrade = function() calls.confirmed = true end,
            setLocalVar = function(_, name, value) calls.playerVar = { name, value } end,
            changeMusic = function(_, channel, song) calls.music[channel] = song end,
            addStatusEffect = function(_, effect, args)
                calls.effect = effect
                calls.effectArgs = args
            end,
        }
        local npc = {
            setLocalVar = function(_, name, value) calls.npcVar = { name, value } end,
        }
        local chest = {
            setAnimationSub = function(_, value) calls.animationSub = value end,
            setStatus = function(_, value) calls.chestStatus = value end,
            resetLocalVars = function() calls.chestVarsReset = true end,
        }

        stub('GetNPCByID', function() return chest end)
        xi.brigandsChart.onEventUpdate(player, 902, 0, npc)

        assert(calls.confirmed)
        assert(calls.npcVar[1] == 'bChartSpawnerID' and calls.npcVar[2] == 123)
        assert(calls.playerVar[1] == 'bChartActive' and calls.playerVar[2] == 1)
        assert(calls.music[0] == 136 and calls.music[1] == 136)
        assert(calls.music[2] == 136 and calls.music[3] == 136)
        assert(calls.effect == xi.effect.LEVEL_RESTRICTION)
        assert(calls.effectArgs.power == 20 and calls.effectArgs.origin == player)
        assert(calls.animationSub == 0 and calls.chestStatus == xi.status.DISAPPEAR and calls.chestVarsReset)
    end)

    it('ignores other event updates', function()
        local calls = 0
        local player = {
            confirmTrade = function() calls = calls + 1 end,
            setLocalVar = function() calls = calls + 1 end,
            changeMusic = function() calls = calls + 1 end,
            addStatusEffect = function() calls = calls + 1 end,
        }
        local npc = { setLocalVar = function() calls = calls + 1 end }

        xi.brigandsChart.onEventUpdate(player, 901, 0, npc)
        xi.brigandsChart.onEventUpdate(player, 902, 1, npc)

        assert(calls == 0)
    end)
end)

describe("Brigand's Chart event finish", function()
    it('sets up the timed chart event after both setup NPCs resolve', function()
        local calls = {}
        local player = {
            showText = function(_, ...) calls.text = { ... } end,
        }
        local npc = {
            setStatus = function(_, value) calls.qmStatus = value end,
            timer = function(_, delay, callback)
                calls.emoteDelay = delay
                calls.emoteCallback = callback
            end,
        }
        local hume = {
            setStatus = function(_, value) calls.humeStatus = value end,
            setAnimation = function(_, value) calls.humeAnimation = value end,
        }
        local shimmering = {
            setStatus = function(_, value) calls.shimmeringStatus = value end,
            timer = function(_, delay, callback)
                calls.shimmerDelay = delay
                calls.shimmerCallback = callback
            end,
            entityAnimationPacket = function(_, value) calls.shimmerAnimation = value end,
        }

        stub('GetNPCByID', function(id)
            if id == zones[xi.zone.BUBURIMU_PENINSULA].npc.BRIGAND_CHART_HUME then
                return hume
            end
            if id == zones[xi.zone.BUBURIMU_PENINSULA].npc.SHIMMERING_POINT then
                return shimmering
            end
        end)
        stub('GetSystemTime', function() return 500 end)
        xi.brigandsChart.onEventFinish(player, 902, 0, npc)

        assert(calls.humeStatus == xi.status.NORMAL and calls.humeAnimation == xi.animation.NONE)
        assert(calls.qmStatus == xi.status.DISAPPEAR and calls.shimmeringStatus == xi.status.NORMAL)
        assert(calls.shimmerDelay == 2000 and calls.emoteDelay == 1000)
        calls.shimmerCallback(shimmering)
        assert(calls.shimmerAnimation == xi.animationString.SHIMMER)
        assert(calls.text[1] == npc and calls.text[2] == 7856 and calls.text[3] == xi.item.PENGUIN_RING)
    end)

    it('does nothing when event setup cannot resolve every required NPC', function()
        local calls = 0
        local player = { showText = function() calls = calls + 1 end }
        local npc = {
            setStatus = function() calls = calls + 1 end,
            timer = function() calls = calls + 1 end,
        }

        stub('GetNPCByID', function() return nil end)
        xi.brigandsChart.onEventFinish(player, 902, 0, npc)
        xi.brigandsChart.onEventFinish(player, 901, 0, npc)

        assert(calls == 0)
    end)
end)
