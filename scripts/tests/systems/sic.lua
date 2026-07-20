require('scripts/actions/mobskills/sic')
describe('Sic mob skill', function()
    it('sets JOBABILITY_FINISH category and USES message', function()
        local sic = require('scripts/actions/mobskills/sic')
        local category, message = nil, nil
        local action = { setCategory = function(_, c) category = c end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(sic.onMobSkillCheck({}, {}, skill) == 0)
        assert(sic.onMobWeaponSkill({}, {}, skill, action) == 0)
        assert(category == xi.action.category.JOBABILITY_FINISH and message == xi.msg.basic.USES)
    end)
end)
