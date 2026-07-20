require('scripts/actions/mobskills/earthbreaker')

describe('Earthbreaker mob skill', function()
    it('uses its Earth magical plan and applies Stun only after processing', function()
        local breaker = require('scripts/actions/mobskills/earthbreaker')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end

        assert(breaker.onMobSkillCheck(target, mob, skill) == 0)
        assert(breaker.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1)
        assert(damage == nil and effect == nil)

        xi.mobskills.processDamage = function() return true end
        assert(breaker.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.EARTH)
        assert(effect[1] == xi.effect.STUN and effect[2] == 1 and effect[3] == 0 and effect[4] == 10)
    end)
end)
