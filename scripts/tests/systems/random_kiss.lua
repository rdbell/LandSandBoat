require('scripts/actions/mobskills/random_kiss')
describe('Random Kiss mob skill', function()
    it('uses magical drain plan with random drain type', function()
        local skill = require('scripts/actions/mobskills/random_kiss')
        local params, drain = nil, nil
        local origM, origD, origDrain, origR = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove, math.random
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobDrainMove = function(mob, target, dtype, amount)
            drain = { dtype, amount }
            return 0
        end
        math.random = function(a, b) return xi.mobskills.drainType.MP end
        local mob = { getMainLvl = function() return 40 end }
        local sk = { setMsg = function() end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(params.fTP[1] == 2.9 and params.skipMagicBonusDiff and params.skipDamageAdjustment)
        assert(drain[1] == xi.mobskills.drainType.MP and drain[2] == 100)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove, math.random = origM, origD, origDrain, origR
    end)
end)
