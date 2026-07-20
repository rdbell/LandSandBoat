require('scripts/actions/mobskills/mind_wall')

describe('Mind Wall mob skill', function()
    it('requires animation sub 3 and applies Magic Shield self-buff returning 0', function()
        local mind = require('scripts/actions/mobskills/mind_wall')
        local buff = xi.mobskills.mobBuffMove
        local params, message, animSub = nil, nil, 0
        local mob = {
            getAnimationSub = function() return animSub end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end

        assert(mind.onMobSkillCheck({}, mob, skill) == 1)
        animSub = 3
        assert(mind.onMobSkillCheck({}, mob, skill) == 0)
        assert(mind.onMobWeaponSkill(mob, {}, skill, {}) == 0)
        xi.mobskills.mobBuffMove = buff

        assert(params[1] == mob and params[2] == xi.effect.MAGIC_SHIELD and params[3] == 3 and params[4] == 0 and params[5] == 30)
        assert(message == 456)
    end)
end)
