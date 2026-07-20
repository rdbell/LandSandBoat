require('scripts/actions/mobskills/lunatic_voice')

describe('Lunatic Voice mob skill', function()
    it('forwards its fixed Mute status result to the skill message', function()
        local lunaticVoice = require('scripts/actions/mobskills/lunatic_voice')
        local status = xi.mobskills.mobStatusEffectMove
        local call, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) call = { ... }; return 456 end

        assert(lunaticVoice.onMobSkillCheck(target, mob, skill) == 0)
        assert(lunaticVoice.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.MUTE)
        assert(call[1] == mob and call[2] == target and call[3] == xi.effect.MUTE and call[4] == 1 and call[5] == 0 and call[6] == 60 and message == 456)

        xi.mobskills.mobStatusEffectMove = status
    end)
end)
