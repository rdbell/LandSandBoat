require('scripts/actions/mobskills/blood_pact')
describe('Blood Pact mob skill', function()
    it('sets JOBABILITY_FINISH and USES message', function()
        local skill = require('scripts/actions/mobskills/blood_pact')
        local category, message = nil, nil
        local action = { setCategory = function(_, v) category = v end }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, action) == 0)
        assert(category == xi.action.category.JOBABILITY_FINISH)
        assert(message == xi.msg.basic.USES)
    end)
end)
