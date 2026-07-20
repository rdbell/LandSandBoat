require('scripts/actions/mobskills/dice_stun')

describe('Dice Stun mob skill', function()
    it('is always available and applies fixed Stun', function()
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.STUN and power == 1 and tick == 0 and duration == 6)
            return 77
        end

        local diceStun = require('scripts/actions/mobskills/dice_stun')
        assert(diceStun.onMobSkillCheck({}, {}, {}) == 0)
        assert(diceStun.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.STUN)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(message == 77)
    end)
end)
