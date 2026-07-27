local impAggro = require('scripts/mixins/families/imp_aggro')

describe('Imp aggro mixin', function()
    it('updates detection only when the Vana’diel hour changes', function()
        local mob = {
            detectionUpdates = 0,
            listeners = {},
            localVars = {},
            mobMods = {},
        }

        function mob:addListener(event, _, callback)
            self.listeners[event] = callback
        end

        function mob:setLocalVar(name, value)
            self.localVars[name] = value
        end

        function mob:getLocalVar(name)
            return self.localVars[name] or 0
        end

        function mob:setMobMod(mod, value)
            self.mobMods[mod] = value
            self.detectionUpdates = self.detectionUpdates + 1
        end

        impAggro(mob)

        assert(xi.mix.imp_aggro.detectionForHour(5) == xi.detects.SIGHT_AND_HEARING)
        assert(xi.mix.imp_aggro.detectionForHour(6) == xi.detects.HEARING)
        assert(xi.mix.imp_aggro.detectionForHour(17) == xi.detects.HEARING)
        assert(xi.mix.imp_aggro.detectionForHour(18) == xi.detects.SIGHT_AND_HEARING)
        assert(xi.mix.imp_aggro.shouldUpdate(6, 6) == false)
        assert(xi.mix.imp_aggro.shouldUpdate(6, 7) == true)

        local hour = VanadielHour()
        mob.listeners.SPAWN(mob)
        assert(mob.localVars.hour == hour)
        assert(mob.mobMods[xi.mobMod.DETECTION] == xi.mix.imp_aggro.detectionForHour(hour))
        assert(mob.detectionUpdates == 1)

        mob.listeners.ROAM_TICK(mob)
        assert(mob.detectionUpdates == 1)

        mob.localVars.hour = (hour + 1) % 24
        mob.listeners.ROAM_TICK(mob)
        assert(mob.localVars.hour == hour)
        assert(mob.mobMods[xi.mobMod.DETECTION] == xi.mix.imp_aggro.detectionForHour(hour))
        assert(mob.detectionUpdates == 2)
    end)
end)
