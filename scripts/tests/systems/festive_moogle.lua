require('scripts/globals/festive_moogle')

describe('Festive Moogle trigger', function()
    it('starts the granted-item event with pending items', function()
        local event = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function(_, name) return name == 'festiveMoogleNomadCap' and 1 or 0 end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.festiveMoogle.onTrigger(player, {})

        assert(event[1] == 380 and event[2] == xi.item.NOMAD_CAP)
    end)

    it('clears the granted item charvar after a successful claim', function()
        local item = nil
        local clearedVar = nil
        local clearedValue = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function(_, name) return name == 'festiveMoogleNomadCap' and 1 or 0 end,
            getFreeSlotsCount = function() return 1 end,
            addItem = function(_, entry) item = entry return true end,
            messageSpecial = function() end,
            setCharVar = function(_, name, value)
                clearedVar = name
                clearedValue = value
            end,
        }

        xi.festiveMoogle.onEventFinish(player, 380, 1, {})

        assert(item.id == xi.item.NOMAD_CAP and item.quantity == 1)
        assert(clearedVar == 'festiveMoogleNomadCap' and clearedValue == 0)
    end)
end)

describe('Festive Moogle pell trade', function()
    it('starts the reward event for an exact Gold Mog Pell trade', function()
        local event = nil
        local tradedPell = nil
        local confirmedItem = nil
        local confirmedQuantity = nil
        local player = {
            getCharVar = function(_, name) return name == '[GUILD]currentGuild' and 1 or 0 end,
            hasItem = function() return false end,
            setLocalVar = function(_, _, value) tradedPell = value end,
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local trade = {
            getSlotCount = function() return 1 end,
            getItemId = function() return xi.item.GOLD_MOG_PELL end,
            hasItemQty = function(_, item, quantity) return item == xi.item.GOLD_MOG_PELL and quantity == 1 end,
            getItemQty = function(_, item) return item == xi.item.GOLD_MOG_PELL and 1 or 0 end,
            confirmItem = function(_, item, quantity)
                confirmedItem = item
                confirmedQuantity = quantity
            end,
        }

        xi.festiveMoogle.onTrade(player, {}, trade)

        assert(tradedPell == xi.item.GOLD_MOG_PELL)
        assert(confirmedItem == xi.item.GOLD_MOG_PELL and confirmedQuantity == 1)
        assert(event[1] == 439 and event[2] == 0 and event[3] == 0 and event[4] == 0 and event[6] == 0)
    end)

    it('returns the traded pell message when the reward event is cancelled', function()
        local message = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getLocalVar = function(_, name) return name == 'tradedPell' and xi.item.GOLD_MOG_PELL or 0 end,
            messageSpecial = function(_, ...) message = { ... } end,
        }

        xi.festiveMoogle.onEventFinish(player, 439, utils.EVENT_CANCELLED_OPTION, {})

        assert(message[1] == 6406 and message[2] == xi.item.GOLD_MOG_PELL)
    end)

    it('gives the selected equipment reward and confirms the trade', function()
        local item = nil
        local confirmed = false
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getFreeSlotsCount = function() return 1 end,
            addItem = function(_, entry) item = entry return true end,
            messageSpecial = function() end,
            confirmTrade = function() confirmed = true end,
        }

        xi.festiveMoogle.onEventFinish(player, 439, 1, {})

        assert(item.id == xi.item.RIDILL and item.quantity == 1)
        assert(confirmed)
    end)
end)
