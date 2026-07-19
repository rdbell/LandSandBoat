require('scripts/actions/mobskills/amplification')

describe('Amplification mob skill', function()
    it('allows use, applies Magic Defense then Magic Attack Boost, and forwards the first message', function()
        local buff = xi.mobskills.mobBuffMove
        local calls, message = {}, nil
        local mob, target, skill = {}, {}, { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) calls[#calls + 1] = { ... }; return 91 end
        local amplification = require('scripts/actions/mobskills/amplification')
        assert(amplification.onMobSkillCheck(target, mob, skill) == 0)
        assert(amplification.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.MAGIC_DEF_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(#calls == 2 and calls[1][2] == xi.effect.MAGIC_DEF_BOOST and calls[2][2] == xi.effect.MAGIC_ATK_BOOST)
        assert(calls[1][3] == 30 and calls[1][4] == 0 and calls[1][5] == 120 and calls[2][3] == 30 and calls[2][4] == 0 and calls[2][5] == 120)
        assert(message == 91)
    end)
end)
