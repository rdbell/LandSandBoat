require('scripts/actions/mobskills/extremely_bad_breath')

describe('Extremely Bad Breath mob skill', function()
    it('misses at or below Death Resistance and otherwise sets HP to zero', function()
        local breath = require('scripts/actions/mobskills/extremely_bad_breath')
        local random = math.random
        local message, hp = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        local target = {
            getMod = function() return 25 end,
            setHP = function(_, value) hp = value end,
        }
        math.random = function(low, high) assert(low == 1 and high == 100); return 25 end
        assert(breath.onMobSkillCheck(target, {}, skill) == 0)
        assert(breath.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(message == xi.msg.basic.SKILL_MISS and hp == nil)
        math.random = function() return 26 end
        assert(breath.onMobWeaponSkill({}, target, skill, {}) == 0)
        math.random = random
        assert(message == xi.msg.basic.FALL_TO_GROUND and hp == 0)
    end)
end)
