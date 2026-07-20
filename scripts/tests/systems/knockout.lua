describe('Knockout mob skill', function()
    it('uses its AGI-scaled Blunt plan and applies Evasion Down only after processing', function()
        local knockout = require('scripts/actions/mobskills/knockout')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, evasion
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) evasion = { ... } end
        assert(knockout.onMobSkillCheck(target, mob, {}) == 0 and knockout.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 4 and params.fTP[2] == 4.5 and params.fTP[3] == 5 and params.accuracyModifier[1] == 50 and params.accuracyModifier[2] == 50 and params.accuracyModifier[3] == 50 and params.agi_wSC == .85 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and evasion == nil)
        xi.mobskills.processDamage = function() return true end
        knockout.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT and evasion[1] == mob and evasion[2] == target and evasion[3] == xi.effect.EVASION_DOWN and evasion[4] == 20 and evasion[5] == 0 and evasion[6] == 30)
    end)
end)
