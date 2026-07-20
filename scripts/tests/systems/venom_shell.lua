require('scripts/actions/mobskills/venom_shell')
describe('Venom Shell mob skill', function()
    it('applies level-scaled poison with floor 16', function()
        local skill = require('scripts/actions/mobskills/venom_shell')
        local status = xi.mobskills.mobStatusEffectMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getMainLvl = function() return 50 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.POISON and message == 456)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.POISON and params[4] == 23 and params[5] == 0 and params[6] == 120)
    end)
end)
