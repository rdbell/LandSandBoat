require('scripts/actions/mobskills/lesson_in_pain')

describe('Lesson in Pain mob skill', function()
    it('uses a no-element magical plan and damages only after processing succeeds', function()
        local lesson = require('scripts/actions/mobskills/lesson_in_pain')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(lesson.onMobSkillCheck(target, mob, {}) == 0)
        assert(lesson.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2.8 and params.fTP[2] == 2.8 and params.fTP[3] == 2.8)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(lesson.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
    end)
end)
