require('scripts/actions/mobskills/blackout')
describe('Blackout mob skill', function()
    it('applies Paralyze Silence Blind with shared random power/duration', function()
        local skill = require('scripts/actions/mobskills/blackout')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local effects, message, rolls = {}, nil, {15, 120}
        local ri = 0
        math.random = function(a, b)
            ri = ri + 1
            if ri == 1 then assert(a == 10 and b == 20); return 15 end
            assert(a == 60 and b == 180)
            return 120
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(effects, { effect, power, tick, duration })
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BLINDNESS)
        math.random = origRandom
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(#effects == 3)
        assert(effects[1][1] == xi.effect.PARALYSIS and effects[1][2] == 15 and effects[1][4] == 120)
        assert(effects[2][1] == xi.effect.SILENCE and effects[2][2] == 1 and effects[2][4] == 120)
        assert(effects[3][1] == xi.effect.BLINDNESS and effects[3][2] == 15 and effects[3][4] == 120)
        assert(message == 242)
    end)
end)
