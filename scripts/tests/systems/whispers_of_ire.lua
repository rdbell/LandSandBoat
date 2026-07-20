require('scripts/actions/mobskills/whispers_of_ire')
describe('Whispers of Ire mob skill', function()
    it('drains unique attributes and messages EFFECT_DRAINED', function()
        local skill = require('scripts/actions/mobskills/whispers_of_ire')
        local drain = xi.mobskills.mobDrainAttribute
        local random = math.random
        local message, calls = nil, {}
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        -- amount=2; effectType picks 1 (STR) then 3 (VIT)
        local picks = { 2, 1, 3 }
        local si = 0
        math.random = function(a, b)
            si = si + 1
            if si == 1 then
                assert(a == 1 and b == 6)
            else
                assert(a == 1 and b == 7)
            end
            return picks[si]
        end
        xi.mobskills.mobDrainAttribute = function(m, t, effect, power, tick, duration)
            calls[#calls + 1] = { effect, power, tick, duration }
        end
        local count = skill.onMobWeaponSkill({}, {}, sk, {})
        xi.mobskills.mobDrainAttribute = drain
        math.random = random
        assert(count == 2 and message == xi.msg.basic.EFFECT_DRAINED)
        assert(calls[1][1] == xi.effect.STR_DOWN and calls[1][2] == 14 and calls[1][3] == 3 and calls[1][4] == 60)
        assert(calls[2][1] == xi.effect.VIT_DOWN)
    end)
end)
