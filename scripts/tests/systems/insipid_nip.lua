describe('Insipid Nip mob skill', function()
    it('uses its Piercing physical plan and drains a random attribute only after processing', function()
        local nip = require('scripts/actions/mobskills/insipid_nip')
        local move, process, drain, random = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainAttribute, math.random
        local params, damage, drained
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainAttribute = function(_, _, ...) drained = { ... } end
        math.random = function(min, max) if min == 0 then assert(max == 6); return 0 end; assert(min == 25 and max == 30); return 25 end
        assert(nip.onMobSkillCheck(target, mob, {}) == 0 and nip.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and drained == nil)
        xi.mobskills.processDamage = function() return true end
        nip.onMobWeaponSkill(mob, target, {}, {})
        math.random = function(min) if min == 0 then return 6 end; return 30 end
        nip.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainAttribute, math.random = move, process, drain, random
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(drained[1] == xi.effect.STR_DOWN + 6 and drained[2] == 30 and drained[3] == 9 and drained[4] == 120)
    end)
end)
