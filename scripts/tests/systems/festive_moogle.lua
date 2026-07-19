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
end)
