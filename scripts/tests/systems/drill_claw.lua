require('scripts/actions/mobskills/drill_claw')

describe('Drill Claw mob skill', function()
    it('uses its piercing physical plan and applies both reductions only after processing', function()
        local drillClaw = require('scripts/actions/mobskills/drill_claw')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(effects, { effect, power, tick, duration })
        end

        assert(drillClaw.onMobSkillCheck({}, {}, {}) == 0)
        assert(drillClaw.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        assert(drillClaw.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(effects[1][1] == xi.effect.MAX_HP_DOWN and effects[1][2] == 50 and effects[1][3] == 0 and effects[1][4] == 60)
        assert(effects[2][1] == xi.effect.MAX_TP_DOWN and effects[2][2] == 50 and effects[2][3] == 0 and effects[2][4] == 60)
    end)
end)
