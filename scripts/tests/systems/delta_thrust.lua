require('scripts/actions/mobskills/delta_thrust')

describe('Delta Thrust mob skill', function()
    it('uses its physical plan and applies Plague only after processed damage', function()
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(effects, { effect, power, tick, duration })
        end
        xi.mobskills.processDamage = function() return false end

        local thrust = require('scripts/actions/mobskills/delta_thrust')
        assert(thrust.onMobSkillCheck({}, {}, {}) == 0)
        assert(thrust.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        assert(thrust.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.mobStatusEffectMove = effectMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(effects[1][1] == xi.effect.PLAGUE and effects[1][2] == 5 and effects[1][3] == 3 and effects[1][4] == 60)
    end)
end)
