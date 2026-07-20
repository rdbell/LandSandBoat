require('scripts/actions/mobskills/slaverous_gale')
describe('Slaverous Gale mob skill', function()
    it('uses earth magical plan with Plague and Slow', function()
        local skill = require('scripts/actions/mobskills/slaverous_gale')
        local params, statuses = nil, {}
        local origM, origD, origS = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 200, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            statuses[#statuses+1] = { e, power, tick, dur }
            return 0
        end
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.element == xi.element.EARTH and params.fTP[1] == 5.0)
        assert(statuses[1][1] == xi.effect.PLAGUE and statuses[2][1] == xi.effect.SLOW and statuses[2][2] == 5000)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
