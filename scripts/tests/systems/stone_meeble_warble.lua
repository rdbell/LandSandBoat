require('scripts/actions/mobskills/stone_meeble_warble')
describe('Stone Meeble Warble mob skill', function()
    it('uses earth magical plan with Petrification and Rasp', function()
        local skill = require('scripts/actions/mobskills/stone_meeble_warble')
        local params, statuses = nil, {}
        local origM, origD, origS = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 200, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            statuses[#statuses+1] = { e, power, tick, dur }
            return 0
        end
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 16.0 and statuses[1][1] == xi.effect.PETRIFICATION and statuses[2][1] == xi.effect.RASP)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = origM, origD, origS
    end)
end)
