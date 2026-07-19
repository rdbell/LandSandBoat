require('scripts/actions/mobskills/dark_sphere')

describe('Dark Sphere mob skill', function()
    it('is always available', function()
        local sphere = require('scripts/actions/mobskills/dark_sphere')

        assert(sphere.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('uses its no-element Dark magical plan and applies Blindness only after processing', function()
        local params, damage, blindness = nil, nil, nil
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
        xi.mobskills.mobStatusEffectMove = function(...) blindness = { ... } end

        local sphere = require('scripts/actions/mobskills/dark_sphere')
        assert(sphere.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == nil and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and blindness == nil)

        xi.mobskills.processDamage = function() return true end
        assert(sphere.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(blindness[3] == xi.effect.BLINDNESS and blindness[4] == 20 and blindness[5] == 0 and blindness[6] == 300)
    end)
end)
