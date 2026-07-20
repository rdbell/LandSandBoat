require('scripts/actions/mobskills/zephyr_arrow')
describe('Zephyr Arrow mob skill', function()
    it('uses piercing plan, breakBind false, and processed Bind', function()
        local skill = require('scripts/actions/mobskills/zephyr_arrow')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, flags, effect = nil, nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType, opts)
                damage = { value, source, attackType, damageType }
                flags = opts
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 40 and params.fTP[1] == 5.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 100 and flags.breakBind == false)
        assert(effect[1] == xi.effect.BIND and effect[2] == 1 and effect[4] == 120)
    end)
end)
