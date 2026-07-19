require('scripts/actions/mobskills/deadly_hold')

describe('Deadly Hold mob skill', function()
    it('is always available', function()
        local hold = require('scripts/actions/mobskills/deadly_hold')

        assert(hold.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('uses its critical physical plan and applies processed damage', function()
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end

        local hold = require('scripts/actions/mobskills/deadly_hold')
        assert(hold.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.attackMultiplier[1] == 2 and params.attackMultiplier[2] == 2 and params.attackMultiplier[3] == 2)
        assert(params.canCrit and params.criticalChance[1] == 0.30 and params.criticalChance[2] == 0.30 and params.criticalChance[3] == 0.30)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(hold.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
