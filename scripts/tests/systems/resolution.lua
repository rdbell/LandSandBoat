require('scripts/actions/mobskills/resolution')
describe('Resolution mob skill', function()
    it('uses its fivefold slashing plan with subsequent-hit fTP and attack multiplier', function()
        local res = require('scripts/actions/mobskills/resolution')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(res.onMobSkillCheck(target, mob, {}) == 0 and res.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 5 and params.fTP[1] == 0.71875 and params.fTPSubsequentHits[3] == 0.96875)
        assert(params.attackMultiplier[1] == 0.85 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        res.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
