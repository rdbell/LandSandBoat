require('scripts/actions/mobskills/earth_shock')

describe('Earth Shock mob skill', function()
    it('uses its physical plan and applies Stun only after processing', function()
        local shock = require('scripts/actions/mobskills/earth_shock')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end

        assert(shock.onMobSkillCheck(target, mob, skill) == 0)
        assert(shock.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 0.5 and params.fTP[2] == 0.5 and params.fTP[3] == 0.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil and effect == nil)

        xi.mobskills.processDamage = function() return true end
        assert(shock.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(effect[1] == xi.effect.STUN and effect[2] == 1 and effect[3] == 0 and effect[4] == 4)
    end)
end)
