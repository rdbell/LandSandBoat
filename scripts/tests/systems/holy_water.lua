require('scripts/actions/mobskills/holy_water')
describe('Holy Water mob skill', function()
    it('deletes Curse Zombie and Doom', function()
        local skill = require('scripts/actions/mobskills/holy_water')
        local deleted = {}
        local target = {
            delStatusEffect = function(_, e) deleted[#deleted+1] = e end,
        }
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 0)
        assert(#deleted == 3)
        assert(deleted[1] == xi.effect.CURSE_I and deleted[2] == xi.effect.CURSE_II and deleted[3] == xi.effect.DOOM)
    end)
end)
