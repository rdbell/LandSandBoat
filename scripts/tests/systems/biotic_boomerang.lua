require('scripts/actions/mobskills/biotic_boomerang')
describe('Biotic Boomerang mob skill', function()
    it('uses Slashing crit plan and processed Plague', function()
        local skill = require('scripts/actions/mobskills/biotic_boomerang')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local origRandom = math.random
        local params, damage, effect = nil, nil, nil
        math.random = function(a, b)
            assert(a == 30 and b == 60)
            return 45
        end
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.canCrit and params.criticalChance[3] == 0.25 and damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        math.random = origRandom
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 90 and effect[1] == xi.effect.PLAGUE and effect[2] == 5 and effect[3] == 3 and effect[4] == 45)
    end)
end)
