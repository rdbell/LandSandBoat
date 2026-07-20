require('scripts/actions/mobskills/molting_burst')

describe('Molting Burst mob skill', function()
    it('uses its Light magical plan, damages only after processing, then self-heals', function()
        local molting = require('scripts/actions/mobskills/molting_burst')
        local move, process, heal = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobHealMove
        local params, damage, healArgs = nil, nil, nil
        local mob = {
            getMainLvl = function() return 75 end,
            getMaxHP = function() return 1000 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobHealMove = function(...)
            healArgs = { ... }
            return 100
        end

        assert(molting.onMobSkillCheck(target, mob, skill) == 0 and molting.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 5 and params.fTP[2] == 5 and params.fTP[3] == 5)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        molting.onMobWeaponSkill(mob, target, skill, {})
        molting.onMobSkillFinalize(mob, skill)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobHealMove = move, process, heal

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
        assert(healArgs[1] == mob and healArgs[2] == 100)
    end)
end)
