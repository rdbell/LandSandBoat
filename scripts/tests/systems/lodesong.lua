require('scripts/actions/mobskills/lodesong')

describe('Lodesong mob skill', function()
    it('rejects silenced casters and otherwise forwards its Weight status result', function()
        local lodesong = require('scripts/actions/mobskills/lodesong')
        local status = xi.mobskills.mobStatusEffectMove
        local call, message, silenced = nil, nil, false
        local mob = { hasStatusEffect = function(_, effect) assert(effect == xi.effect.SILENCE); return silenced end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) call = { ... }; return 456 end

        assert(lodesong.onMobSkillCheck(target, mob, skill) == 0)
        assert(lodesong.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.WEIGHT)
        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.WEIGHT and call[4] == 75 and call[5] == 0 and call[6] == 30 and message == 456)

        silenced = true
        assert(lodesong.onMobSkillCheck(target, mob, skill) == 1)
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
