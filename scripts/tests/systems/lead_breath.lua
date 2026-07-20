require('scripts/actions/mobskills/lead_breath')

describe('Lead Breath mob skill', function()
    it('sets the status result as its message and returns Weight', function()
        local leadBreath = require('scripts/actions/mobskills/lead_breath')
        local status = xi.mobskills.mobStatusEffectMove
        local applied, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... }; return 321 end

        assert(leadBreath.onMobSkillCheck(target, mob, skill) == 0)
        assert(leadBreath.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.WEIGHT)

        xi.mobskills.mobStatusEffectMove = status
        assert(applied[1] == mob and applied[2] == target and applied[3] == xi.effect.WEIGHT)
        assert(applied[4] == 50 and applied[5] == 0 and applied[6] == 300)
        assert(message == 321)
    end)
end)
