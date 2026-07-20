require('scripts/actions/mobskills/disseverment')

describe('Disseverment mob skill', function()
    it('uses its five-hit physical plan and applies Poison only after damage processing', function()
        local disseverment = require('scripts/actions/mobskills/disseverment')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, poison = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            poison = { effect, power, tick, duration }
        end

        assert(disseverment.onMobSkillCheck({}, {}, {}) == 0)
        assert(disseverment.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 5)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5)
        assert(damage == nil and poison == nil)

        xi.mobskills.processDamage = function() return true end
        assert(disseverment.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(poison[1] == xi.effect.POISON and poison[2] == 19 and poison[3] == 3 and poison[4] == 180)
    end)
end)
