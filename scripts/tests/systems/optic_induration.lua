require('scripts/actions/mobskills/optic_induration')

describe('Optic Induration mob skill', function()
    it('uses its blunt physical plan with crit, petrify, and enmity reset after processing', function()
        local skill = require('scripts/actions/mobskills/optic_induration')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams, enmityTarget = nil, nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 77 end,
            resetEnmity = function(_, target) enmityTarget = target end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        math.random = function(low, high)
            assert(low == 15 and high == 60)
            return 42
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0 and skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 6 and params.canCrit)
        assert(params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and statusParams == nil and enmityTarget == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(statusParams[3] == xi.effect.PETRIFICATION and statusParams[4] == 1 and statusParams[5] == 0 and statusParams[6] == 42)
        assert(enmityTarget == target)
    end)
end)
