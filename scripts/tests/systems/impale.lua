describe('Impale mob skill', function()
    it('uses its physical plan and branches to Paralysis or notorious Poison with enmity reset only after processing', function()
        local impale = require('scripts/actions/mobskills/impale')
        local move, process, effect = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status, resets = nil, nil, nil, 0
        local mob = { getWeaponDmg = function() return 77 end, isMobType = function() return false end, resetEnmity = function() resets = resets + 1 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) status = { ... } end
        assert(impale.onMobSkillCheck(target, mob, {}) == 0 and impale.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.attackMultiplier[1] == 2 and params.attackMultiplier[2] == 2 and params.attackMultiplier[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and status == nil and resets == 0)
        xi.mobskills.processDamage = function() return true end
        impale.onMobWeaponSkill(mob, target, {}, {})
        assert(status[1] == xi.effect.PARALYSIS and status[2] == 20 and status[3] == 0 and status[4] == 120 and resets == 0)
        mob.isMobType = function() return true end
        impale.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(status[1] == xi.effect.POISON and status[2] == 20 and status[3] == 0 and status[4] == 120 and resets == 1)
    end)
end)
