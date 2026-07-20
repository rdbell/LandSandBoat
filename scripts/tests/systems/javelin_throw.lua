describe('Javelin Throw mob skill', function()
    it('requires an armed DRG and uses its ranged plan before setting animation sub two', function()
        local throw = require('scripts/actions/mobskills/javelin_throw')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage, final
        local animation, job = 0, xi.job.DRG
        local mob = { getAnimationSub = function() return animation end, getMainJob = function() return job end, getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { setFinalAnimationSub = function(_, value) final = value end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end

        assert(throw.onMobSkillCheck(target, mob, skill) == 0)
        animation = 1; assert(throw.onMobSkillCheck(target, mob, skill) == 1)
        animation = 0; job = xi.job.WAR; assert(throw.onMobSkillCheck(target, mob, skill) == 1)
        job = xi.job.DRG; assert(throw.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.skipParry and params.skipGuard and params.skipBlock and damage == nil and final == 2)

        xi.mobskills.processDamage = function() return true end
        throw.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
