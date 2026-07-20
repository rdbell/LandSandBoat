require('scripts/actions/mobskills/dice_disease')

describe('Dice Disease mob skill', function()
    it('is always available and applies fixed Disease', function()
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.DISEASE and power == 1 and tick == 0 and duration == 180)
            return 77
        end

        local diceDisease = require('scripts/actions/mobskills/dice_disease')
        assert(diceDisease.onMobSkillCheck({}, {}, {}) == 0)
        assert(diceDisease.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DISEASE)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(message == 77)
    end)
end)
