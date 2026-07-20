require('scripts/actions/mobskills/light_shot')

describe('Light Shot mob skill', function()
    it('forwards the Sleep I status result to its message and returns Sleep I', function()
        local lightShot = require('scripts/actions/mobskills/light_shot')
        local status = xi.mobskills.mobStatusEffectMove
        local call, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) call = { ... }; return 456 end

        assert(lightShot.onMobSkillCheck(target, mob, skill) == 0)
        assert(lightShot.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = status

        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.SLEEP_I)
        assert(call[4] == 1 and call[5] == 0 and call[6] == 60 and message == 456)
    end)
end)
