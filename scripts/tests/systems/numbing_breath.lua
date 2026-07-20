require('scripts/actions/mobskills/numbing_breath')

describe('Numbing Breath mob skill', function()
    it('uses Ice breath parameters and paralyzes only after processing', function()
        local breath = require('scripts/actions/mobskills/numbing_breath')
        local move, process, status, random = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local params, damage, paralysis = nil, nil, nil
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) paralysis = { ... } end
        math.random = function(low, high)
            assert(low == 120 and high == 180)
            return 150
        end
        assert(breath.onMobSkillCheck(target, {}, {}) == 0 and breath.onMobWeaponSkill({}, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.0625 and params.damageCap == 500 and params.bonusDamage == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.ICE)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.ICE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and paralysis == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, status, random
        assert(damage[1] == 123 and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.ICE)
        assert(paralysis[3] == xi.effect.PARALYSIS and paralysis[4] == 20 and paralysis[5] == 0 and paralysis[6] == 150)
    end)
end)
