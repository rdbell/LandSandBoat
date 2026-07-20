require('scripts/actions/mobskills/smokebomb')
describe('Smoke Bomb mob skill', function()
    it('applies Blindness status', function()
        local bomb = require('scripts/actions/mobskills/smokebomb')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(bomb.onMobSkillCheck({}, {}, skill) == 0)
        assert(bomb.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BLINDNESS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.BLINDNESS and params[4] == 20 and params[6] == 120 and message == 456)
    end)
end)
