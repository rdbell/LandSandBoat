require('scripts/actions/mobskills/target_analysis')
describe('Target Analysis mob skill', function()
    it('gates on skill list/HPP/animation and drains attributes at 40%', function()
        local skill = require('scripts/actions/mobskills/target_analysis')
        local drain = xi.mobskills.mobDrainAttribute
        local random = math.random
        local mob = {
            getMobMod = function(_, mod) return 54 end,
            getHPP = function() return 20 end,
            getAnimationSub = function() return 0 end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        mob.getHPP = function() return 25 end
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.getMobMod = function() return 727 end
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        local calls, n = {}, 0
        math.random = function(a, b)
            assert(a == 0 and b == 100)
            n = n + 1
            if n == 1 or n == 3 then return 10 end -- drain
            return 90 -- skip
        end
        xi.mobskills.mobDrainAttribute = function(m, t, effect, power, tick, duration)
            calls[#calls + 1] = { effect, power, tick, duration }
            return 100
        end
        local sk = { setMsg = function() end }
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 2)
        xi.mobskills.mobDrainAttribute = drain
        math.random = random
        assert(calls[1][1] == xi.effect.STR_DOWN and calls[2][1] == xi.effect.VIT_DOWN)
        assert(calls[1][2] == 10 and calls[1][3] == 3 and calls[1][4] == 60)
    end)
end)
