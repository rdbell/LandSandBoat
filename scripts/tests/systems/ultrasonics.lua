require('scripts/actions/mobskills/ultrasonics')
describe('Ultrasonics mob skill', function()
    it('applies evasion down for 180s', function()
        local skill = require('scripts/actions/mobskills/ultrasonics')
        local status = xi.mobskills.mobStatusEffectMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.EVASION_DOWN and message == 456)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.EVASION_DOWN and params[4] == 25 and params[6] == 180)
    end)
end)
