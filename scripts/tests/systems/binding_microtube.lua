require('scripts/actions/mobskills/binding_microtube')
describe('Binding Microtube mob skill', function()
    it('uses NONE magical plan, breakBind false, and processed Bind', function()
        local skill = require('scripts/actions/mobskills/binding_microtube')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, flags, effect = nil, nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = {
            takeDamage = function(_, v, s, a, d, opts)
                damage = { v, s, a, d }
                flags = opts
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 6.45 and params.element == xi.element.NONE and damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 100 and flags.breakBind == false)
        assert(effect[1] == xi.effect.BIND and effect[4] == 60)
    end)
end)
