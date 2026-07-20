require('scripts/actions/mobskills/memory_of_lightning')

describe('Memory Of Lightning mob skill', function()
    it('uses its Thunder magical plan and damages only after processing', function()
        local memory = require('scripts/actions/mobskills/memory_of_lightning')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end

        assert(memory.onMobSkillCheck(target, mob, skill) == 0 and memory.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        memory.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
    end)
end)
