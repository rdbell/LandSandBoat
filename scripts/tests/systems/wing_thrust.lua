require('scripts/actions/mobskills/wing_thrust')
describe('Wing Thrust mob skill', function()
    it('uses four-hit physical plan and processed Slow with random duration', function()
        local skill = require('scripts/actions/mobskills/wing_thrust')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local params, damage, effect = nil, nil, nil
        math.random = function(a, b)
            assert(a == 30 and b == 60)
            return 45
        end
        local mob = { getWeaponDmg = function() return 55 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 88, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 88)
        assert(params.numHits == 4 and params.fTP[1] == 0.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4)
        assert(damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 88)
        math.random = origRandom
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 88)
        assert(effect[1] == xi.effect.SLOW and effect[2] == 3437 and effect[4] == 45)
    end)
end)
