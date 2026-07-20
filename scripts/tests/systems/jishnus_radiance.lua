describe("Jishnu's Radiance mob skill", function()
    it('uses its three-hit ranged critical plan and damages only after processing', function()
        local radiance = require('scripts/actions/mobskills/jishnus_radiance')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        assert(radiance.onMobSkillCheck(target, mob, {}) == 0 and radiance.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3 and params.fTP[1] == 1.75 and params.fTP[2] == 1.75 and params.fTP[3] == 1.75 and params.skipParry and params.skipGuard and params.skipBlock and params.criticalChance[1] == .15 and params.criticalChance[2] == .2 and params.criticalChance[3] == .25 and params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        radiance.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
    end)
end)
