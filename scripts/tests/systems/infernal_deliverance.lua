describe('Infernal Deliverance mob skill', function()
    it('uses its Blunt physical plan and applies zero-power Stun only after processing', function()
        local deliverance = require('scripts/actions/mobskills/infernal_deliverance')
        local move, process, effect = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, stun
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) stun = { ... } end
        assert(deliverance.onMobSkillCheck(target, mob, {}) == 0 and deliverance.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil and stun == nil)
        xi.mobskills.processDamage = function() return true end
        deliverance.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(stun[1] == xi.effect.STUN and stun[2] == 0 and stun[3] == 0 and stun[4] == 10)
    end)
end)
