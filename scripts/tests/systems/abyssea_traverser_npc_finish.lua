require('scripts/globals/abyssea')

describe('Abyssea traverser NPC finish', function()
    local oldHeld = xi.abyssea.getHeldTraverserStones
    local oldCap = xi.abyssea.getTraverserCap

    before_each(function()
        xi.abyssea.getHeldTraverserStones = function() return 1 end
        xi.abyssea.getTraverserCap = function() return 3 end
    end)

    after_each(function()
        xi.abyssea.getHeldTraverserStones = oldHeld
        xi.abyssea.getTraverserCap = oldCap
    end)

    it('claims available stones and preserves the inclusive key-item loop', function()
        local claimed = 0
        local keyItems = {}
        local p = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getAvailableTraverserStones = function() return 2 end,
            addClaimedTraverserStones = function(_, amount) claimed = amount end,
            addKeyItem = function(_, keyItem) table.insert(keyItems, keyItem) end,
            messageSpecial = function() end,
        }
        xi.abyssea.traverserNPCOnEventFinish(p, 405, 6)
        assert(claimed == 2 and #keyItems == 3)
        assert(keyItems[1] == xi.ki.TRAVERSER_STONE1 and keyItems[3] == xi.ki.TRAVERSER_STONE1 + 2)
    end)

    it('does nothing for another option', function()
        local changed = false
        local p = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            addClaimedTraverserStones = function() changed = true end,
        }
        xi.abyssea.traverserNPCOnEventFinish(p, 405, 5)
        assert(changed == false)
    end)
end)
