require('scripts/actions/mobskills/last_stand')

describe('Last Stand mob skill', function()
    it('uses a two-hit ranged plan and only damages after processing succeeds', function()
        local lastStand = require('scripts/actions/mobskills/last_stand')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(lastStand.onMobSkillCheck(target, mob, {}) == 0)
        assert(lastStand.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2.125 and params.fTP[3] == 2.25)
        assert(params.fTPSubsequentHits[1] == 2 and params.fTPSubsequentHits[2] == 2.125 and params.fTPSubsequentHits[3] == 2.25)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.attackType == xi.attackType.RANGED and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(lastStand.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.RANGED and damage[4] == xi.damageType.PIERCING)
    end)
end)
