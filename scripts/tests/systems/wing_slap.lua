require('scripts/actions/mobskills/wing_slap')
describe('Wing Slap mob skill', function()
    it('delegates Apkallu check and uses four-hit Blunt plan with processed Stun', function()
        local skill = require('scripts/actions/mobskills/wing_slap')
        local canUse = xi.apkallu.canUseAbility
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect, threshold = nil, nil, nil, nil
        xi.apkallu.canUseAbility = function(_, t) threshold = t; return 0 end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(threshold == 20)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.numHits == 4 and params.fTP[1] == 0.25 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4)
        assert(params.damageType == xi.damageType.BLUNT and damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        xi.apkallu.canUseAbility = canUse
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 70)
        assert(effect[1] == xi.effect.STUN and effect[2] == 1 and effect[4] == 4)
    end)
end)
