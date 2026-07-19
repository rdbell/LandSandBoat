require('scripts/actions/mobskills/dark_mist')

describe('Dark Mist mob skill', function()
    it('is available only while flying', function()
        local mist = require('scripts/actions/mobskills/dark_mist')

        assert(mist.onMobSkillCheck({}, { getAnimationSub = function() return 5 end }, {}) == 0)
        assert(mist.onMobSkillCheck({}, { getAnimationSub = function() return 4 end }, {}) == 1)
    end)

    it('uses its Dark magical plan and applies damage and Weight only after processing', function()
        local params, damage, weight = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) weight = { ... } end

        local mist = require('scripts/actions/mobskills/dark_mist')
        assert(mist.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and weight == nil)

        xi.mobskills.processDamage = function() return true end
        assert(mist.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(weight[3] == xi.effect.WEIGHT and weight[4] == 50 and weight[5] == 0 and weight[6] == 60)
    end)
end)
