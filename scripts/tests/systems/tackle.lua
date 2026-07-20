require('scripts/actions/mobskills/tackle')
describe('Tackle mob skill', function()
    it('uses blunt physical plan with Stun on process', function()
        local skill = require('scripts/actions/mobskills/tackle')
        local params, status = nil, nil
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 60, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.damageType == xi.damageType.BLUNT and params.fTP[1] == 1.5)
        assert(status[1] == xi.effect.STUN and status[4] == 4)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
