require('scripts/actions/mobskills/vaccine')

describe('Vaccine mob skill', function()
    it('allows use and removes Plague from its target', function()
        local vaccine = require('scripts/actions/mobskills/vaccine')
        local removedEffect = nil
        local target = {
            delStatusEffect = function(_, effect)
                removedEffect = effect
            end,
        }

        assert(vaccine.onMobSkillCheck(target, {}, {}) == 0)
        assert(vaccine.onMobWeaponSkill({}, target, {}, {}) == 0)
        assert(removedEffect == xi.effect.PLAGUE)
    end)
end)
