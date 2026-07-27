require('scripts/globals/abyssea')

describe('Abyssea traverser NPC update', function()
    local oldTime = GetSystemTime
    local oldAbyssiteTotal = xi.abyssea.getAbyssiteTotal

    before_each(function()
        xi.abyssea.getAbyssiteTotal = function() return 2 end
    end)

    after_each(function()
        xi.abyssea.getAbyssiteTotal = oldAbyssiteTotal
    end)

    it('updates the remaining recharge minutes for the traverser menu', function()
        local updated
        local epoch = oldTime() - 18 * 3600
        local p = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getTraverserEpoch = function() return epoch end,
            getClaimedTraverserStones = function() return 1 end,
            updateEvent = function(_, ...) updated = { ... } end,
        }
        xi.abyssea.traverserNPCOnUpdate(p, 405, 3)
        assert(updated[5] >= 1079 and updated[5] <= 1080, 'remaining=' .. tostring(updated[5]))
    end)

    it('does not update for another option', function()
        local updated = false
        local p = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            updateEvent = function() updated = true end,
        }
        xi.abyssea.traverserNPCOnUpdate(p, 405, 2)
        assert(updated == false)
    end)
end)
