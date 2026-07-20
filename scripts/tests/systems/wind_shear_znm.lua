require('scripts/actions/mobskills/wind_shear_znm')
describe('Wind Shear ZNM mob skill', function()
    it('uses its Wind plan and processed Weight', function()
        local skill = require('scripts/actions/mobskills/wind_shear_znm')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.25 and params.fTP[3] == 1.75)
        assert(params.element == xi.element.WIND and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 90)
        assert(effect[1] == xi.effect.WEIGHT and effect[2] == 50 and effect[3] == 0 and effect[4] == 120)
    end)
end)
