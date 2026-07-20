require('scripts/actions/mobskills/smouldering_swarm')
describe('Smouldering Swarm mob skill', function()
    it('uses fire magical plan with Burn on process', function()
        local skill = require('scripts/actions/mobskills/smouldering_swarm')
        local params, status = nil, nil
        local origM, origD, origS = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.element == xi.element.FIRE and params.fTP[1] == 2.0)
        assert(status[1] == xi.effect.BURN and status[2] == 10 and status[4] == 90)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
