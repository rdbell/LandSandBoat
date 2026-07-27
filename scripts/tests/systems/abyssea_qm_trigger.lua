require('scripts/globals/abyssea')

describe('Abyssea QM trigger', function()
    it('reports trade requirements before checking a pop mob', function()
        local started
        local p = {
            getZoneID = function() return xi.zone.ABYSSEA_ALTEPA end,
            startEvent = function(_, ...) started = { ... } end,
        }
        assert(xi.abyssea.qmOnTrigger(p, {}, 0, {}, { 1, 2 }))
        assert(started[1] == 1010 and started[2] == 1 and started[3] == 2 and started[4] == 0)
    end)
end)
