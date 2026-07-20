require('scripts/actions/mobskills/phase_shift_3')
describe('Phase Shift 3 mob skill', function()
    it('always fails skill check and applies Stun then Bind on process', function()
        local skill = require('scripts/actions/mobskills/phase_shift_3')
        local params, statuses = nil, {}
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            statuses[#statuses+1] = { e, power, tick, dur }
            return 0
        end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 7.0)
        assert(statuses[1][1] == xi.effect.STUN and statuses[2][1] == xi.effect.BIND and statuses[2][4] == 30)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
