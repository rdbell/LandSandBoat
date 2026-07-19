require('scripts/actions/mobskills/bad_breath')

describe('Bad Breath mob skill', function()
    it('uses its Earth breath plan and applies ordered ailments only after bind-preserving damage processing', function()
        local params, damage, effects = nil, nil, {}
        local mob, target = {}, { takeDamage = function(_, ...) damage = { ... } end }
        local breathMove = xi.mobskills.mobBreathMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobBreathMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) effects[#effects + 1] = { ... } end

        local breath = require('scripts/actions/mobskills/bad_breath')
        assert(breath.onMobSkillCheck({}, {}, {}) == 0)
        assert(breath.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.12 and params.damageCap == 500 and params.bonusDamage == 0 and params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.EARTH and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        assert(breath.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobBreathMove = breathMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.EARTH and damage[5].breakBind == false)
        assert(#effects == 7)
        assert(effects[1][3] == xi.effect.SLOW and effects[1][4] == 1250 and effects[1][5] == 0 and effects[1][6] == 60)
        assert(effects[2][3] == xi.effect.POISON and effects[2][4] == 1 and effects[2][5] == 3 and effects[2][6] == 60)
        assert(effects[3][3] == xi.effect.SILENCE and effects[3][4] == 1 and effects[3][5] == 0 and effects[3][6] == 60)
        assert(effects[4][3] == xi.effect.PARALYSIS and effects[4][4] == 15 and effects[4][5] == 0 and effects[4][6] == 60)
        assert(effects[5][3] == xi.effect.BIND and effects[5][4] == 1 and effects[5][5] == 0 and effects[5][6] == 60)
        assert(effects[6][3] == xi.effect.BLINDNESS and effects[6][4] == 15 and effects[6][5] == 0 and effects[6][6] == 60)
        assert(effects[7][3] == xi.effect.WEIGHT and effects[7][4] == 50 and effects[7][5] == 0 and effects[7][6] == 60)
    end)
end)
