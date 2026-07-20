require('scripts/actions/mobskills/Reward')
describe('Reward mob skill', function()
    it('no-ops without pet and fully heals pet with USES', function()
        local skill = require('scripts/actions/mobskills/Reward')
        local message, setHP = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getPet = function() return nil end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == nil)
        assert(message == nil)
        local pet = {
            getMaxHP = function() return 500 end,
            setHP = function(_, v) setHP = v end,
        }
        mob.getPet = function() return pet end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        assert(setHP == 500 and message == xi.msg.basic.USES)
    end)
end)
