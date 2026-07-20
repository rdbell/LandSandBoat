require('scripts/actions/mobskills/dragonfall')

describe('Dragonfall mob skill', function()
    it('uses its physical plan and applies non-breaking damage plus Bind only after processing', function()
        local dragonfall = require('scripts/actions/mobskills/dragonfall')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, bind = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            bind = { effect, power, tick, duration }
        end

        assert(dragonfall.onMobSkillCheck({}, {}, {}) == 0)
        assert(dragonfall.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2.7 and params.fTP[2] == 2.7 and params.fTP[3] == 2.7)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and bind == nil)

        xi.mobskills.processDamage = function() return true end
        assert(dragonfall.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING and damage[5].breakBind == false)
        assert(bind[1] == xi.effect.BIND and bind[2] == 1 and bind[3] == 0 and bind[4] == 30)
    end)
end)
