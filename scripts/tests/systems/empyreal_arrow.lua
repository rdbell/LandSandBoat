require('scripts/actions/mobskills/empyreal_arrow')

describe('Empyreal Arrow mob skill', function()
    it('uses its ranged plan and damages only after processing succeeds', function()
        local arrow = require('scripts/actions/mobskills/empyreal_arrow')
        local rangedMove, processDamage = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end

        assert(arrow.onMobSkillCheck({}, {}, {}) == 0)
        assert(arrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2.75 and params.fTP[3] == 3)
        assert(params.attackMultiplier[1] == 2 and params.attackMultiplier[2] == 2 and params.attackMultiplier[3] == 2)
        assert(params.skipParry and params.skipGuard and params.skipBlock and params.accuracyModifier[1] == 100 and params.accuracyModifier[2] == 100 and params.accuracyModifier[3] == 100)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(arrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = rangedMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
    end)
end)
