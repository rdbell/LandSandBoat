require('scripts/actions/mobskills/shield_bash_1')
describe('Shield Bash 1 mob skill', function()
    it('uses blunt physical plan with Stun on process', function()
        local skill = require('scripts/actions/mobskills/shield_bash_1')
        local params, status, cat = nil, nil, nil
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 50, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        local action = { setCategory = function(_, c) cat = c end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, action) == 50)
        assert(cat == xi.action.category.WEAPONSKILL_FINISH and params.fTP[1] == 1.0)
        assert(status[1] == xi.effect.STUN and status[4] == 7)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
