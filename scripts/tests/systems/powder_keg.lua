require('scripts/actions/mobskills/powder_keg')
describe('Powder Keg mob skill', function()
    it('uses physical plan with DEFENSE_DOWN and MAGIC_DEF_DOWN', function()
        local skill = require('scripts/actions/mobskills/powder_keg')
        local params, statuses = nil, {}
        local origP, origD, origS = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 30, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            statuses[#statuses+1] = { e, power, tick, dur }
            return 0
        end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 30)
        assert(params.fTP[1] == 0.3)
        assert(statuses[1][1] == xi.effect.DEFENSE_DOWN and statuses[2][1] == xi.effect.MAGIC_DEF_DOWN)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origP, origD, origS
    end)
end)
