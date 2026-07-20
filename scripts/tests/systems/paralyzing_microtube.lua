require('scripts/actions/mobskills/paralyzing_microtube')

describe('Paralyzing Microtube mob skill', function()
    it('uses its magical plan and applies Paralysis after processing', function()
        local microtube = require('scripts/actions/mobskills/paralyzing_microtube')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(microtube.onMobSkillCheck(target, mob, {}) == 0 and microtube.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 12.25 and params.element == xi.element.NONE)
        assert(params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        microtube.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
        assert(statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 20 and statusParams[5] == 0 and statusParams[6] == 60)
    end)
end)
