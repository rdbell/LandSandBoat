local amphiptere = require('scripts/mixins/families/amphiptere')

local function newMob()
    local mob = {
        abilities = {},
        animationSub = -1,
        hidden = nil,
        listeners = {},
        localVars = {},
        untargetable = nil,
    }

    function mob:addListener(event, _, callback)
        self.listeners[event] = callback
    end

    function mob:hideName(value)
        self.hidden = value
    end

    function mob:setUntargetable(value)
        self.untargetable = value
    end

    function mob:setAnimationSub(value)
        self.animationSub = value
    end

    function mob:setLocalVar(name, value)
        self.localVars[name] = value
    end

    function mob:getLocalVar(name)
        return self.localVars[name] or 0
    end

    function mob:useMobAbility(skill)
        table.insert(self.abilities, skill)
    end

    return mob
end

describe('Amphiptere mixin', function()
    it('applies visibility states and runs the Reaving Wind aura loop', function()
        local mob = newMob()
        amphiptere(mob)

        mob.listeners.SPAWN(mob)
        assert(mob.hidden == true and mob.untargetable == true and mob.animationSub == 1)

        mob.listeners.ENGAGE(mob)
        assert(mob.hidden == false and mob.untargetable == false and mob.animationSub == 0)

        mob.listeners.DISENGAGE(mob)
        assert(mob.hidden == true and mob.untargetable == true and mob.animationSub == 1)

        assert(xi.mix.amphiptere.auraEndTime(100) == 120)
        assert(xi.mix.amphiptere.shouldContinueAura(119, 120) == true)
        assert(xi.mix.amphiptere.shouldContinueAura(120, 120) == false)

        local reavingWind = { getID = function() return xi.mobSkill.REAVING_WIND end }
        mob.listeners.WEAPONSKILL_USE(mob, nil, reavingWind)
        assert(mob.animationSub == 2)
        assert(mob:getLocalVar('auraEndTime') >= GetSystemTime() + 19)

        mob.listeners.WEAPONSKILL_STATE_EXIT(mob, xi.mobSkill.REAVING_WIND, false)
        assert(mob.abilities[#mob.abilities] == xi.mobSkill.REAVING_WIND_KNOCKBACK)

        mob:setLocalVar('auraEndTime', GetSystemTime() + 60)
        mob.listeners.WEAPONSKILL_STATE_EXIT(mob, xi.mobSkill.REAVING_WIND_KNOCKBACK, false)
        assert(mob.abilities[#mob.abilities] == xi.mobSkill.REAVING_WIND_KNOCKBACK)

        mob:setLocalVar('auraEndTime', 0)
        mob.listeners.WEAPONSKILL_STATE_EXIT(mob, xi.mobSkill.REAVING_WIND_KNOCKBACK, false)
        assert(mob:getLocalVar('auraEndTime') == 0)
        assert(mob.animationSub == 0)
    end)
end)
