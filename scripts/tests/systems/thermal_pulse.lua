require('scripts/actions/mobskills/thermal_pulse')
describe('Thermal Pulse mob skill', function()
    it('admits open form and applies Blindness with TP duration', function()
        local skill = require('scripts/actions/mobskills/thermal_pulse')
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
        local mob = {
            getAnimationSub = function() return 0 end,
            getMainLvl = function() return 50 end,
        }
        local sk = { getTP = function() return 1000 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 4.5 and status[1] == xi.effect.BLINDNESS and status[2] == 100 and status[4] == 30)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
