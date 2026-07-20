describe('Immortal Anathema mob skill', function()
    it('allows use, sets the Curse I result message, and returns Curse I', function()
        local anathema = require('scripts/actions/mobskills/immortal_anathema')
        local status = xi.mobskills.mobStatusEffectMove
        local message, args
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(_, _, ...)
            args = { ... }
            return 777
        end

        assert(anathema.onMobSkillCheck({}, {}, skill) == 0)
        assert(anathema.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.CURSE_I)
        xi.mobskills.mobStatusEffectMove = status

        assert(message == 777)
        assert(args[1] == xi.effect.CURSE_I and args[2] == 25 and args[3] == 0 and args[4] == 300)
    end)
end)
