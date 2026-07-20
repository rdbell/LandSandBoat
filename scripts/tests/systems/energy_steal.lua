require('scripts/actions/mobskills/energy_steal')
describe('Energy Steal mob skill', function()
    it('uses dark magical MP drain plan fTP 1/2.1/3.4', function()
        local skill = require('scripts/actions/mobskills/energy_steal')
        local params, drain = nil, nil
        local origM, origD, origDrain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobDrainMove = function(mob, target, dtype, amount)
            drain = { dtype, amount }
            return 0
        end
        local mob = { getMainLvl = function() return 50 end }
        local sk = { setMsg = function() end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(params.element == xi.element.DARK and params.fTP[3] == 3.4 and params.skipStoneSkin)
        assert(drain[1] == xi.mobskills.drainType.MP and drain[2] == 100)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = origM, origD, origDrain
    end)
end)
