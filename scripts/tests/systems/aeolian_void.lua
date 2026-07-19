require('scripts/actions/mobskills/aeolian_void')

describe('Aeolian Void mob skill', function()
    it('uses its Wind magical plan and applies damage and effects only after processing', function()
        local params, damage, effects = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) effects[#effects + 1] = { ... } end

        local void = require('scripts/actions/mobskills/aeolian_void')
        assert(void.onMobSkillCheck({}, {}, {}) == 0)
        assert(void.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        assert(void.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
        assert(#effects == 2)
        assert(effects[1][3] == xi.effect.SILENCE and effects[1][4] == 1 and effects[1][5] == 0 and effects[1][6] == 180)
        assert(effects[2][3] == xi.effect.BLINDNESS and effects[2][4] == 50 and effects[2][5] == 0 and effects[2][6] == 180)
    end)
end)
