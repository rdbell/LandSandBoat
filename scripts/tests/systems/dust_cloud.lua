require('scripts/actions/mobskills/dust_cloud')

describe('Dust Cloud mob skill', function()
    it('uses its Earth magical plan and applies Blindness only after processing', function()
        local cloud = require('scripts/actions/mobskills/dust_cloud')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, blindness = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) blindness = { effect, power, tick, duration } end
        assert(cloud.onMobSkillCheck({}, {}, {}) == 0)
        assert(cloud.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.EARTH and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and blindness == nil)
        xi.mobskills.processDamage = function() return true end
        assert(cloud.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.EARTH)
        assert(blindness[1] == xi.effect.BLINDNESS and blindness[2] == 30 and blindness[3] == 0 and blindness[4] == 120)
    end)
end)
