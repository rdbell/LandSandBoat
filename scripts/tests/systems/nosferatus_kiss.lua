require('scripts/actions/mobskills/nosferatus_kiss')
describe("Nosferatu's Kiss mob skill", function()
    it('uses magical IGNORE_SHADOWS plan and drains HP/MP/TP on process', function()
        local skill = require('scripts/actions/mobskills/nosferatus_kiss')
        local params, drains = nil, {}
        local origM, origD, origDrain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 108, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobDrainMove = function(mob, target, dtype, amount)
            drains[#drains+1] = { dtype, amount }
            return 0
        end
        local target = { takeDamage = function() end }
        local mob = { getMainLvl = function() return 85 end }
        local sk = { setMsg = function() end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 108)
        assert(params.fTP[1] == 1.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(#drains == 3 and drains[1][1] == xi.mobskills.drainType.HP)
        assert(drains[2][1] == xi.mobskills.drainType.MP and drains[3][1] == xi.mobskills.drainType.TP)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = origM, origD, origDrain
    end)
end)
