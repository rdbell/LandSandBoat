describe('Ink Cloud mob skill', function()
    it('uses TP-scaled Blindness power, a random duration, and returns Blindness', function()
        local cloud = require('scripts/actions/mobskills/ink_cloud')
        local status, random = xi.mobskills.mobStatusEffectMove, math.random
        local message, args
        local skill = { getTP = function() return 2000 end, setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) args = { ... }; return 777 end
        math.random = function(min, max) assert(min == 30 and max == 60); return 45 end
        assert(cloud.onMobSkillCheck({}, {}, skill) == 0 and cloud.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BLINDNESS)
        xi.mobskills.mobStatusEffectMove, math.random = status, random
        assert(message == 777 and args[1] == xi.effect.BLINDNESS and args[2] == 55 and args[3] == 0 and args[4] == 45)
    end)
end)
