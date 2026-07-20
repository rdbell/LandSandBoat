require('scripts/actions/mobskills/poison_breath_dragon_2')
describe('Poison Breath Dragon 2 mob skill', function()
    it('uses water breath with fixed Poison power 50', function()
        local skill = require('scripts/actions/mobskills/poison_breath_dragon_2')
        local params, status = nil, nil
        local origB, origD, origS, origR = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        xi.mobskills.mobBreathMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 40, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        math.random = function() return 150 end
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 40)
        assert(params.percentMultipier == 0.10 and status[2] == 50 and status[4] == 150)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = origB, origD, origS, origR
    end)
end)
