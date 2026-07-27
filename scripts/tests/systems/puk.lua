local puk = require('scripts/mixins/families/puk')

describe('Puk mixin', function()
    it('gains full TP only from the current elemental-day damage type', function()
        local days = {
            { xi.day.FIRESDAY, xi.damageType.FIRE },
            { xi.day.EARTHSDAY, xi.damageType.EARTH },
            { xi.day.WATERSDAY, xi.damageType.WATER },
            { xi.day.WINDSDAY, xi.damageType.WIND },
            { xi.day.ICEDAY, xi.damageType.ICE },
            { xi.day.LIGHTNINGDAY, xi.damageType.THUNDER },
            { xi.day.LIGHTSDAY, xi.damageType.LIGHT },
            { xi.day.DARKSDAY, xi.damageType.DARK },
        }

        for _, entry in ipairs(days) do
            assert(xi.mix.puk.damageTypeForDay(entry[1]) == entry[2])
            assert(xi.mix.puk.shouldGainTP(entry[1], entry[2]) == true)
            assert(xi.mix.puk.shouldGainTP(entry[1], xi.damageType.NONE) == false)
        end

        local mob = { listeners = {}, tp = 0 }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:addTP(amount) self.tp = self.tp + amount end
        puk(mob)

        local day = VanadielDayOfTheWeek()
        mob.listeners.TAKE_DAMAGE(mob, 0, nil, nil, xi.mix.puk.damageTypeForDay(day))
        assert(mob.tp == 1000)
        mob.listeners.TAKE_DAMAGE(mob, 0, nil, nil, xi.damageType.NONE)
        assert(mob.tp == 1000)
    end)
end)
