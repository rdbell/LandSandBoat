require('scripts/actions/mobskills/stellar_burst')
describe('Stellar Burst mob skill', function()
    it('uses magical plan with Silence and enmity reset', function()
        local skill = require('scripts/actions/mobskills/stellar_burst')
        local params, status, reset = nil, nil, false
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
        local mob = { getMainLvl = function() return 50 end, resetEnmity = function() reset = true end }
        local target = { takeDamage = function() end, isTrust = function() return false end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.0 and status[1] == xi.effect.SILENCE and reset)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
