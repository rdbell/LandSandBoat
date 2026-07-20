require('scripts/actions/mobskills/poison_breath_dragon_1')
describe('Poison Breath Dragon 1 mob skill', function()
    it('uses water breath plan with level-scaled Poison on process', function()
        local skill = require('scripts/actions/mobskills/poison_breath_dragon_1')
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
        math.random = function(a, b) return 150 end
        local mob = { getMainLvl = function() return 25 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.percentMultipier == 0.10 and params.damageCap == 400)
        assert(status[1] == xi.effect.POISON and status[2] == 6 and status[3] == 3 and status[4] == 150)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = origB, origD, origS, origR
    end)
end)
