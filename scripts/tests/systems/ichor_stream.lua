describe('Ichor Stream mob skill', function()
    it('allows use, sets the Poison result message, and returns Poison', function()
        local stream = require('scripts/actions/mobskills/ichor_stream')
        local status = xi.mobskills.mobStatusEffectMove
        local message, args
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(_, _, ...)
            args = { ... }
            return 777
        end

        assert(stream.onMobSkillCheck({}, {}, skill) == 0)
        assert(stream.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.POISON)
        xi.mobskills.mobStatusEffectMove = status

        assert(message == 777)
        assert(args[1] == xi.effect.POISON and args[2] == 5 and args[3] == 0 and args[4] == 120)
    end)
end)
