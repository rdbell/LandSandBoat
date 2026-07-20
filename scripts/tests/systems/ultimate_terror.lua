require('scripts/actions/mobskills/ultimate_terror')
describe('Ultimate Terror mob skill', function()
    it('drains shuffled attributes and messages EFFECT_DRAINED or MISS', function()
        local skill = require('scripts/actions/mobskills/ultimate_terror')
        local drain = xi.mobskills.mobDrainAttribute
        local random, shuffle = math.random, utils.shuffle
        local message, drainCalls = nil, {}
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        math.random = function(a, b) assert(a == 2 and b == 4); return 2 end
        utils.shuffle = function(t) return t end -- keep STR,DEX,... order
        xi.mobskills.mobDrainAttribute = function(m, t, effect, power, tick, duration)
            drainCalls[#drainCalls + 1] = { effect, power, tick, duration }
            return 1 -- success
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 2 and message == xi.msg.basic.EFFECT_DRAINED)
        assert(drainCalls[1][1] == xi.effect.STR_DOWN and drainCalls[1][2] == 15 and drainCalls[1][4] == 60)
        assert(drainCalls[2][1] == xi.effect.DEX_DOWN)
        xi.mobskills.mobDrainAttribute = function() return 0 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0 and message == xi.msg.basic.SKILL_MISS)
        xi.mobskills.mobDrainAttribute = drain
        math.random, utils.shuffle = random, shuffle
    end)
end)
