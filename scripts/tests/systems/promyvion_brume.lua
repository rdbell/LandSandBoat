require('scripts/actions/mobskills/promyvion_brume')
describe('Promyvion Brume mob skill', function()
    it('selects craver element and applies Poison on process', function()
        local skill = require('scripts/actions/mobskills/promyvion_brume')
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
        local mob = {
            getMainLvl = function() return 40 end,
            getModelId = function() return 1134 end,
            getAnimationSub = function() return 14 end,
        }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 88)
        assert(params.element == xi.element.WATER and params.fTP[1] == 3)
        assert(status[1] == xi.effect.POISON and status[2] == 9 and status[4] == 180)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
