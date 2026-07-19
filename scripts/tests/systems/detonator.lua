require('scripts/actions/mobskills/detonator')

describe('Detonator mob skill', function()
    it('uses its ranged physical plan and applies processed damage', function()
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobRangedMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        local detonator = require('scripts/actions/mobskills/detonator')
        assert(detonator.onMobSkillCheck({}, {}, {}) == 0)
        assert(detonator.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 1.5 and params.fTP[2] == 2 and params.fTP[3] == 2.5)
        assert(params.attackMultiplier[1] == 2 and params.attackMultiplier[2] == 2 and params.attackMultiplier[3] == 2)
        assert(params.skipParry and params.skipGuard and params.skipBlock and params.accuracyModifier[1] == 100 and params.accuracyModifier[2] == 100 and params.accuracyModifier[3] == 100)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(detonator.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
    end)
end)
