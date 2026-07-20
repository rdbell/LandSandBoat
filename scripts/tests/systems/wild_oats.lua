require('scripts/actions/mobskills/wild_oats')
describe('Wild Oats mob skill', function()
    it('uses its piercing plan and processed VIT Down', function()
        local skill = require('scripts/actions/mobskills/wild_oats')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { getWeaponDmg = function() return 60 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 60 and params.numHits == 1 and params.fTP[1] == 1.0)
        assert(params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 80)
        assert(effect[1] == xi.effect.VIT_DOWN and effect[2] == 10 and effect[3] == 9 and effect[4] == 120)
    end)
end)
