require('scripts/actions/mobskills/tartaric_sigil')
describe('Tartaric Sigil mob skill', function()
    it('uses dark magical plan with TP-scaled Amnesia', function()
        local skill = require('scripts/actions/mobskills/tartaric_sigil')
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
        local sk = { getTP = function() return 3000 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.baseDamage == 150 and params.fTP[1] == 3.75)
        assert(status[1] == xi.effect.AMNESIA and status[4] == 5)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
