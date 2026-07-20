require('scripts/actions/mobskills/steal')
describe('Steal mob skill', function()
    it('unequips one random slot with USES', function()
        local skill = require('scripts/actions/mobskills/steal')
        local msg, cat, count = nil, nil, nil
        local orig = xi.mobskills.unequipRandomSlots
        xi.mobskills.unequipRandomSlots = function(target, n) count = n end
        local action = { setCategory = function(_, c) cat = c end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, action) == 0)
        assert(count == 1 and cat == xi.action.category.WEAPONSKILL_FINISH and msg == xi.msg.basic.USES)
        xi.mobskills.unequipRandomSlots = orig
    end)
end)
