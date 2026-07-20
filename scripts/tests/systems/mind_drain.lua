require('scripts/actions/mobskills/mind_drain')

describe('Mind Drain mob skill', function()
    it('always allows use and applies its MND Down status', function()
        local mind = require('scripts/actions/mobskills/mind_drain')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(...)
            params = { ... }
            return 456
        end

        assert(mind.onMobSkillCheck(target, mob, skill) == 0)
        assert(mind.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.MND_DOWN)
        xi.mobskills.mobStatusEffectMove = status

        assert(params[1] == mob and params[2] == target and params[3] == xi.effect.MND_DOWN)
        assert(params[4] == 14 and params[5] == 3 and params[6] == 300)
        assert(message == 456)
    end)
end)
