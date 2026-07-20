require('scripts/actions/mobskills/dice_sleep')

describe('Dice Sleep mob skill', function()
    it('is always available and applies fixed Sleep I', function()
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.SLEEP_I and power == 1 and tick == 0 and duration == 30)
            return 77
        end

        local diceSleep = require('scripts/actions/mobskills/dice_sleep')
        assert(diceSleep.onMobSkillCheck({}, {}, {}) == 0)
        assert(diceSleep.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(message == 77)
    end)
end)
