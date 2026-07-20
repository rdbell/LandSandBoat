require('scripts/actions/mobskills/meditate')

describe('Meditate mob skill', function()
    it('sets job-ability finish, applies fixed Meditate, and reports Uses', function()
        local meditate = require('scripts/actions/mobskills/meditate')
        local effect, message, category = nil, nil, nil
        local mob = { addStatusEffect = function(_, value, options) effect = { value, options } end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        local action = { setCategory = function(_, value) category = value end }

        assert(meditate.onMobSkillCheck(target, mob, skill) == 0)
        assert(meditate.onMobWeaponSkill(mob, target, skill, action) == xi.effect.MEDITATE)
        assert(category == xi.action.category.JOBABILITY_FINISH and effect[1] == xi.effect.MEDITATE)
        assert(effect[2].power == 20 and effect[2].duration == 15 and effect[2].tick == 3 and effect[2].origin == mob and effect[2].icon == 0)
        assert(message == xi.msg.basic.USES)
    end)
end)
