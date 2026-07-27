local tauri = require('scripts/mixins/families/tauri')

describe('Tauri mixin', function()
    it('allows Mortal Ray once per life and records only Mortal Ray', function()
        assert(xi.mix.tauri.canUseRayWithLocalVar(0) == true)
        assert(xi.mix.tauri.canUseRayWithLocalVar(1) == false)
        assert(xi.mix.tauri.canUseRayWithLocalVar(2) == true)
        assert(xi.mix.tauri.shouldRecordRay(xi.mobSkill.MORTAL_RAY_1) == true)
        assert(xi.mix.tauri.shouldRecordRay(xi.mobSkill.MORTAL_RAY_1 + 1) == false)

        local mob = { listeners = {}, localVars = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:setLocalVar(name, value) self.localVars[name] = value end

        tauri(mob)
        assert(xi.mix.tauri.canUseRay(mob) == true)
        mob.listeners.WEAPONSKILL_USE(mob, nil, { getID = function() return xi.mobSkill.MORTAL_RAY_1 + 1 end })
        assert(xi.mix.tauri.canUseRay(mob) == true)
        mob.listeners.WEAPONSKILL_USE(mob, nil, { getID = function() return xi.mobSkill.MORTAL_RAY_1 end })
        assert(xi.mix.tauri.canUseRay(mob) == false)
    end)
end)
