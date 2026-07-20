require('scripts/actions/mobskills/ectosmash')

describe('Ectosmash mob skill', function()
    it('uses its one-hit physical plan and damages only after processing succeeds', function()
        local ectosmash = require('scripts/actions/mobskills/ectosmash')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(ectosmash.onMobSkillCheck(target, mob, skill) == 0)
        assert(ectosmash.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.attackMultiplier[1] == 1.5 and params.attackMultiplier[2] == 1.5 and params.attackMultiplier[3] == 1.5)
        assert(params.canCrit and params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(ectosmash.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
