local ladybug = require('scripts/mixins/families/ladybug')

describe('Ladybug mixin', function()
    it('changes phase only at day and night transitions', function()
        local night = xi.mix.ladybug.phasePlan(18, 0, 170, 1173)
        assert(night.phase == 1 and night.noAggro == 1 and night.roamCool == 10)
        assert(night.evasionDelta == -15 and night.accuracyDelta == -15 and night.delay == -400 and night.skillList == 1173)
        assert(xi.mix.ladybug.phasePlan(5, 1, 170, 1173) == nil)

        local day = xi.mix.ladybug.phasePlan(6, 1, 170, 1173)
        assert(day.phase == 0 and day.noAggro == 0 and day.roamCool == 0)
        assert(day.evasionDelta == 15 and day.accuracyDelta == 15 and day.delay == 0 and day.skillList == 170)
        assert(xi.mix.ladybug.phasePlan(17, 0, 170, 1173) == nil)

        local mob = { listeners = {}, localVars = {}, mobMods = {}, mods = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:setLocalVar(name, value) self.localVars[name] = value end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:setMobMod(mod, value) self.mobMods[mod] = value end
        function mob:addMod(mod, value) self.mods[mod] = (self.mods[mod] or 0) + value end
        function mob:delMod(mod, value) self.mods[mod] = (self.mods[mod] or 0) - value end
        function mob:setMod(mod, value) self.mods[mod] = value end

        ladybug(mob)
        mob.listeners.SPAWN(mob)
        assert(mob.localVars['[ladybug]daySkillList'] == 170 and mob.localVars['[ladybug]nightSkillList'] == 1173)
    end)
end)
