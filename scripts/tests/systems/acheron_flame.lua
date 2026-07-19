require('scripts/actions/mobskills/acheron_flame')

describe('Acheron Flame mob skill', function()
    it('uses Fire magical parameters and damages and burns only after processing', function()
        local move, process, effectMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, burn = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) burn = { ... } end
        local flame = require('scripts/actions/mobskills/acheron_flame')
        assert(flame.onMobSkillCheck(target, mob, {}) == 0 and flame.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 12.5 and params.fTP[2] == 12.5 and params.fTP[3] == 12.5)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1 and damage == nil and burn == nil)
        xi.mobskills.processDamage = function() return true end
        assert(flame.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
        assert(burn[3] == xi.effect.BURN and burn[4] == 30 and burn[5] == 3 and burn[6] == 60)
    end)
end)
