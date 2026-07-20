require('scripts/actions/mobskills/stifling_tantara')
describe('Stifling Tantara mob skill', function()
    it('refuses hornless Imps and applies Silence on process', function()
        local skill = require('scripts/actions/mobskills/stifling_tantara')
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
        local mob = {
            getAnimationSub = function() return 0 end,
            getSpecies = function() return xi.mobSpecies.IMP end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        mob.getAnimationSub = function() return 5 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 3.0 and status[1] == xi.effect.SILENCE and status[4] == 60)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
