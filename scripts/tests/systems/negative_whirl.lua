require('scripts/actions/mobskills/negative_whirl')
describe('Negative Whirl mob skill', function()
    it('selects element from model and applies Slow on process', function()
        local skill = require('scripts/actions/mobskills/negative_whirl')
        local params, status = nil, nil
        local origM, origD, origS = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 88, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            status = { e, power, tick, dur }
            return 0
        end
        local target = { takeDamage = function() end }
        local mob = {
            getMainLvl = function() return 40 end,
            isNM = function() return false end,
            getModelId = function() return 1123 end,
            getAnimationSub = function() return 14 end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 88)
        assert(params.element == xi.element.WATER and params.fTP[1] == 2)
        assert(status[1] == xi.effect.SLOW and status[2] == 8500 and status[4] == 60)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
