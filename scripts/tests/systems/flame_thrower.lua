describe('Flame Thrower mob skill', function()
    it('uses its level-capped Fire breath plan and applies its processed effects', function()
        local thrower = require('scripts/actions/mobskills/flame_thrower')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, plague, removed, reset = nil, nil, nil, false, nil
        local mainLevel, processed, hasResDown = 64, false, true
        local mob = { getMainLvl = function() return mainLevel end, setLocalVar = function(_, name, value) reset = { name, value } end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            hasStatusEffect = function() return hasResDown end,
            delStatusEffectSilent = function(_, effect) removed = effect end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return processed end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) plague = { ... } end
        assert(thrower.onMobSkillCheck(target, mob, {}) == 0 and thrower.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.05 and params.damageCap == 490 and params.bonusDamage == 0 and params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.FIRE and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and plague == nil and not removed and reset[1] == 'nuclearWaste' and reset[2] == 0)
        mainLevel, processed, hasResDown = 65, true, true
        assert(thrower.onMobWeaponSkill(mob, target, {}, {}) == 123 and params.damageCap == 750)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.FIRE)
        assert(plague[1] == xi.effect.PLAGUE and plague[2] == 5 and plague[3] == 3 and plague[4] == 120 and removed == xi.effect.ELEMENTALRES_DOWN)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
    end)
end)
