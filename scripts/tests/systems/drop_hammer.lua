require('scripts/actions/mobskills/drop_hammer')

describe('Drop Hammer mob skill', function()
    it('uses its physical plan and applies Bind only after processing', function()
        local hammer = require('scripts/actions/mobskills/drop_hammer')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, bind, reset = nil, nil, nil, false
        local mob = { getWeaponDmg = function() return 77 end, getPool = function() return 1 end, resetEnmity = function() reset = true end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) bind = { effect, power, tick, duration } end

        assert(hammer.onMobSkillCheck({}, {}, {}) == 0)
        assert(hammer.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2.4 and params.fTP[2] == 2.4 and params.fTP[3] == 2.4)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and bind == nil and not reset)

        xi.mobskills.processDamage = function() return true end
        assert(hammer.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT and damage[5].breakBind == false)
        assert(bind[1] == xi.effect.BIND and bind[2] == 1 and bind[3] == 0 and bind[4] == 60 and not reset)

        mob.getPool = function() return xi.mobPool.FAHRAFAHR_THE_BLOODIED end
        assert(hammer.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(reset)
    end)
end)
