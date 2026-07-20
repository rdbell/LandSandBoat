require('scripts/actions/mobskills/mow')
describe('Mow mob skill', function()
    it('uses three-hit physical plan with Poison on process', function()
        local skill = require('scripts/actions/mobskills/mow')
        local params, status = nil, nil
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 70, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local target = { takeDamage = function() end }
        local mob = { getWeaponDmg = function() return 30 end }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.numHits == 3 and params.fTP[1] == 0.5)
        assert(status[1] == xi.effect.POISON and status[2] == 25 and status[3] == 3 and status[4] == 30)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
