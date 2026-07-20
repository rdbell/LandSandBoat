require('scripts/actions/mobskills/dreadstorm')

describe('Dreadstorm mob skill', function()
    it('uses its Dark magical plan and applies damage plus Terror only after processing', function()
        local dreadstorm = require('scripts/actions/mobskills/dreadstorm')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, terror = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            terror = { effect, power, tick, duration }
        end

        assert(dreadstorm.onMobSkillCheck({}, {}, {}) == 0)
        assert(dreadstorm.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77)
        assert(params.fTP[1] == 10 and params.fTP[2] == 10 and params.fTP[3] == 10)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and terror == nil)

        xi.mobskills.processDamage = function() return true end
        assert(dreadstorm.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(terror[1] == xi.effect.TERROR and terror[2] == 1 and terror[3] == 0 and terror[4] == 10)
    end)
end)
