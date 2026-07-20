require('scripts/actions/mobskills/boiling_point')
describe('Boiling Point mob skill', function()
    it('applies MAGIC_DEF_DOWN and returns the effect', function()
        local skill = require('scripts/actions/mobskills/boiling_point')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg = nil, nil
        local skillObj = {
            setMsg = function(_, m) msg = m end,
        }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, {}, skillObj, {}) == xi.effect.MAGIC_DEF_DOWN)
        assert(status[1] == xi.effect.MAGIC_DEF_DOWN and status[2] == 50 and status[4] == 120)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
