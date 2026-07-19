require('scripts/actions/mobskills/lowing')

describe('Lowing mob skill', function()
    it('always allows use and applies fixed Plague parameters', function()
        local lowing = require('scripts/actions/mobskills/lowing')
        local oldMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) args = { ... }; return 321 end
        assert(lowing.onMobSkillCheck({}, {}, skill) == 0)
        assert(lowing.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PLAGUE)
        xi.mobskills.mobStatusEffectMove = oldMove
        assert(args[3] == xi.effect.PLAGUE and args[4] == 5 and args[5] == 0 and args[6] == 60)
        assert(message == 321)
    end)
end)
