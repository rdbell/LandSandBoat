require('scripts/actions/mobskills/viper_bite')
describe('Viper Bite mob skill', function()
    it('uses double-attack physical plan with TP-scaled Poison', function()
        local skill = require('scripts/actions/mobskills/viper_bite')
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
        local sk = { getTP = function() return 1000 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(params.attackMultiplier[1] == 2.0 and status[1] == xi.effect.POISON and status[2] == 3 and status[4] == 90)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
