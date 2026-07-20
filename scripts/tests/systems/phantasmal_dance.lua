require('scripts/actions/mobskills/phantasmal_dance')
describe('Phantasmal Dance mob skill', function()
    it('uses physical plan with Bind on process', function()
        local skill = require('scripts/actions/mobskills/phantasmal_dance')
        local params, status = nil, nil
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 80, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(status[1] == xi.effect.BIND and status[2] == 1 and status[4] == 30)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
