require('scripts/actions/mobskills/chemical_bomb')
describe('Chemical Bomb mob skill', function()
    it('applies Elegy then Slow', function()
        local skill = require('scripts/actions/mobskills/chemical_bomb')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local statuses, msg = {}, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            statuses[#statuses+1] = { effect, power, tick, duration }
            return 100 + #statuses
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SLOW)
        assert(#statuses == 2)
        assert(statuses[1][1] == xi.effect.ELEGY and statuses[1][2] == 5000 and statuses[1][4] == 120)
        assert(statuses[2][1] == xi.effect.SLOW and statuses[2][2] == 5000)
        assert(msg == 102)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
