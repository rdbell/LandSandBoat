require('scripts/actions/mobskills/riddle')
describe('Riddle mob skill', function()
    it('applies INT-scaled Max MP Down', function()
        local riddle = require('scripts/actions/mobskills/riddle')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local target = { getStat = function(_, mod) assert(mod == xi.mod.INT); return 10 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(riddle.onMobSkillCheck(target, {}, skill) == 0)
        assert(riddle.onMobWeaponSkill({}, target, skill, {}) == xi.effect.MAX_MP_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.MAX_MP_DOWN and params[4] == 9 and params[5] == 0 and params[6] == 60)
        assert(message == 456)
    end)
end)
