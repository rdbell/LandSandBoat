describe('Jump 6 mob skill', function()
    it('sets JOBABILITY_FINISH and uses VIT-scaled fTP only after processing', function()
        local jump = require('scripts/actions/mobskills/jump_6')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, category
        local mob = { getWeaponDmg = function() return 77 end, getStat = function(_, stat) assert(stat == xi.mod.VIT); return 64 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill, action = {}, { setCategory = function(_, value) category = value end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        assert(jump.onMobSkillCheck(target, mob, skill) == 0 and jump.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(category == xi.action.category.JOBABILITY_FINISH and params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1.25 and params.fTP[2] == 1.25 and params.fTP[3] == 1.25 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        jump.onMobWeaponSkill(mob, target, skill, action)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
