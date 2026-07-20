require('scripts/actions/mobskills/light_blade')

describe('Light Blade mob skill', function()
    it('uses its ranged Slashing plan and distance-scaled damage only after processing', function()
        local blade = require('scripts/actions/mobskills/light_blade')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local distance = 10
        local mob = { getWeaponDmg = function() return 77 end, checkDistance = function() return distance end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(blade.onMobSkillCheck(target, mob, {}) == 0)
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 6 and params.fTP[2] == 6 and params.fTP[3] == 6)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil)

        distance = -1
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 100)
        distance = 100
        xi.mobskills.processDamage = function() return true end
        assert(blade.onMobWeaponSkill(mob, target, {}, {}) == 20)

        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 20 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
