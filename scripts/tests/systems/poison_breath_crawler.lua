require('scripts/actions/mobskills/poison_breath_crawler')
describe('Poison Breath Crawler mob skill', function()
    it('uses water breath plan with Poison on process', function()
        local skill = require('scripts/actions/mobskills/poison_breath_crawler')
        local params, status = nil, nil
        local origB, origD, origS = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobBreathMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 50, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 50)
        assert(params.percentMultipier == 0.103 and params.damageCap == 405 and params.element == xi.element.WATER)
        assert(status[1] == xi.effect.POISON and status[2] == 1 and status[3] == 3 and status[4] == 60)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origB, origD, origS
    end)
end)
