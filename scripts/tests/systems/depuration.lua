require('scripts/actions/mobskills/depuration')

describe('Depuration mob skill', function()
    it('requires an erasable target effect, erases all mob effects, and reports the count', function()
        local erased, message = nil, nil
        local targetEffect = xi.effect.NONE
        local target = { eraseStatusEffect = function() return targetEffect end }
        local mob = { eraseAllStatusEffect = function() erased = 3; return erased end }
        local skill = { setMsg = function(_, value) message = value end }

        local depuration = require('scripts/actions/mobskills/depuration')
        assert(depuration.onMobSkillCheck(target, {}, {}) == 1)
        targetEffect = xi.effect.POISON
        assert(depuration.onMobSkillCheck(target, {}, {}) == 0)
        assert(depuration.onMobWeaponSkill(mob, target, skill, {}) == 3)
        assert(erased == 3 and message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
