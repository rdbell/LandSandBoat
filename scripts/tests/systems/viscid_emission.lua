require('scripts/actions/mobskills/viscid_emission')
describe('Viscid Emission mob skill', function()
    it('applies amnesia with subpower 100 for 60s', function()
        local skill = require('scripts/actions/mobskills/viscid_emission')
        local status = xi.mobskills.mobStatusEffectMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.AMNESIA and message == 456)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.AMNESIA and params[4] == 1 and params[6] == 60 and params[7] == 100)
    end)
end)
