describe('Feeble Bleat mob skill', function()
    it('allows use, forwards its Paralysis message, and returns Paralysis', function()
        local feebleBleat = require('scripts/actions/mobskills/feeble_bleat')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 777
        end
        assert(feebleBleat.onMobSkillCheck({}, {}, {}) == 0)
        assert(feebleBleat.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(params[1] == xi.effect.PARALYSIS and params[2] == 25 and params[3] == 0 and params[4] == 90)
        assert(message == 777)
    end)
end)
