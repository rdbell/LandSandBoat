describe('Foul Breath mob skill', function()
    it('excludes zone 5 and applies Fire breath damage and Disease only after processing', function()
        local breath = require('scripts/actions/mobskills/foul_breath')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, disease = nil, nil, nil
        local mob = { getZoneID = function() return 1 end }
        local forbidden = { getZoneID = function() return 5 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) disease = { ... } end
        assert(breath.onMobSkillCheck(target, forbidden, {}) == 1 and breath.onMobSkillCheck(target, mob, {}) == 0 and breath.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == .083 and params.damageCap == 500 and params.bonusDamage == 0 and params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0 and params.resistStat == xi.mod.INT and params.element == xi.element.FIRE and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and disease == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.FIRE and disease[1] == mob and disease[2] == target and disease[3] == xi.effect.DISEASE and disease[4] == 1 and disease[5] == 0 and disease[6] == 180)
    end)
end)
