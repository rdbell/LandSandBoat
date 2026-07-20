require('scripts/actions/mobskills/venom_breath')
describe('Venom Breath mob skill', function()
    it('applies poison power 50 for 60s', function()
        local skill = require('scripts/actions/mobskills/venom_breath')
        local status = xi.mobskills.mobStatusEffectMove
        local params = nil
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... } end
        skill.onMobWeaponSkill({}, {}, {}, {})
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.POISON and params[4] == 50 and params[5] == 3 and params[6] == 60)
    end)
end)
