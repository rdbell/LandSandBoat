require('scripts/actions/mobskills/toxic_spit')
describe('Toxic Spit mob skill', function()
    it('applies level-scaled poison and returns POISON', function()
        local skill = require('scripts/actions/mobskills/toxic_spit')
        local status = xi.mobskills.mobStatusEffectMove
        local statusParams = nil
        local mob = { getMainLvl = function() return 50 end }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        assert(skill.onMobWeaponSkill(mob, {}, {}, {}) == xi.effect.POISON)
        xi.mobskills.mobStatusEffectMove = status
        assert(statusParams[3] == xi.effect.POISON and statusParams[4] == 13 and statusParams[5] == 3 and statusParams[6] == 180)
    end)
end)
