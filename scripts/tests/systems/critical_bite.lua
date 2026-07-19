require('scripts/actions/mobskills/critical_bite')

describe('Critical Bite mob skill', function()
    it('uses fixed critical physical parameters and damages only after processing succeeds', function()
        local criticalBite = require('scripts/actions/mobskills/critical_bite')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(criticalBite.onMobSkillCheck(target, mob, skill) == 0)
        assert(criticalBite.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.attackMultiplier[1] == 1.5 and params.attackMultiplier[2] == 1.5 and params.attackMultiplier[3] == 1.5)
        assert(params.canCrit and params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(criticalBite.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
