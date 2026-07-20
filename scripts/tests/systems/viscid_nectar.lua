require('scripts/actions/mobskills/viscid_nectar')
describe('Viscid Nectar mob skill', function()
    it('applies slow power 10000 for 120s', function()
        local skill = require('scripts/actions/mobskills/viscid_nectar')
        local status = xi.mobskills.mobStatusEffectMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SLOW and message == 456)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SLOW and params[4] == 10000 and params[6] == 120)
    end)
end)
