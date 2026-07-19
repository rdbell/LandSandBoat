require('scripts/actions/mobskills/dice_curse')

describe('Dice Curse mob skill', function()
    it('is always available and applies fixed Curse I', function()
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.CURSE_I and power == 30 and tick == 0 and duration == 300)
            return 77
        end

        local diceCurse = require('scripts/actions/mobskills/dice_curse')
        assert(diceCurse.onMobSkillCheck({}, {}, {}) == 0)
        assert(diceCurse.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.CURSE_I)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(message == 77)
    end)
end)
