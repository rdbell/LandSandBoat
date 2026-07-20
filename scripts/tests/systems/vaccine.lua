require('scripts/actions/mobskills/vaccine')
describe('Vaccine mob skill', function()
    it('removes plague and returns 0', function()
        local skill = require('scripts/actions/mobskills/vaccine')
        local del = nil
        local target = { delStatusEffect = function(_, e) del = e end }
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 0 and del == xi.effect.PLAGUE)
    end)
end)
