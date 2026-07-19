require('scripts/actions/mobskills/demonic_howl')

describe('Demonic Howl mob skill', function()
    it('is always available and applies fixed Slow', function()
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.SLOW and power == 5000 and tick == 0 and duration == 240)
            return 77
        end

        local howl = require('scripts/actions/mobskills/demonic_howl')
        assert(howl.onMobSkillCheck({}, {}, {}) == 0)
        assert(howl.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLOW)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(message == 77)
    end)
end)
