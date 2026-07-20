require('scripts/actions/mobskills/dice_poison')

describe('Dice Poison mob skill', function()
    it('is always available and applies fixed Poison', function()
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.POISON and power == 15 and tick == 0 and duration == 60)
            return 77
        end

        local dicePoison = require('scripts/actions/mobskills/dice_poison')
        assert(dicePoison.onMobSkillCheck({}, {}, {}) == 0)
        assert(dicePoison.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.POISON)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(message == 77)
    end)
end)
