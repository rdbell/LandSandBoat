require('scripts/actions/mobskills/mordant_rime')
describe('Mordant Rime mob skill', function()
    it('uses two-hit physical plan with Weight on process', function()
        local skill = require('scripts/actions/mobskills/mordant_rime')
        local params, status = nil, nil
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 99, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local target = { takeDamage = function() end }
        local mob = { getWeaponDmg = function() return 40 end }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 99)
        assert(params.numHits == 2 and params.fTP[1] == 3.0)
        assert(status[1] == xi.effect.WEIGHT and status[2] == 25 and status[4] == 60)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
