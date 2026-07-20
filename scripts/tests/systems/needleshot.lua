require('scripts/actions/mobskills/needleshot')

describe('Needleshot mob skill', function()
    it('uses its ranged plan and damages only after processing succeeds', function()
        local shot = require('scripts/actions/mobskills/needleshot')
        local rangedMove, processDamage = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(shot.onMobSkillCheck({}, {}, {}) == 0)
        assert(shot.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(shot.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = rangedMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
    end)
end)
