describe('Incensed Pummel mob skill', function()
    it('uses its captured physical plan and applies a random attribute-down effect only after processing', function()
        local pummel = require('scripts/actions/mobskills/incensed_pummel')
        local move, process, effect, random = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local params, damage, status
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) status = { ... } end
        math.random = function(min, max) assert(min == 0 and max == 6); return 0 end
        assert(pummel.onMobSkillCheck(target, mob, {}) == 0 and pummel.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and status == nil)
        xi.mobskills.processDamage = function() return true end
        pummel.onMobWeaponSkill(mob, target, {}, {})
        assert(status[1] == xi.effect.STR_DOWN and status[2] == 20 and status[3] == 9 and status[4] == 120)
        math.random = function() return 6 end
        pummel.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, effect, random
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(status[1] == xi.effect.STR_DOWN + 6 and status[2] == 20 and status[3] == 9 and status[4] == 120)
    end)
end)
