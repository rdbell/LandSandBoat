require('scripts/actions/mobskills/decimation')

describe('Decimation mob skill', function()
    it('uses its three-hit accuracy-scaled physical plan and applies processed damage', function()
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        local decimation = require('scripts/actions/mobskills/decimation')
        assert(decimation.onMobSkillCheck({}, {}, {}) == 0)
        assert(decimation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3)
        assert(params.fTP[1] == 1.25 and params.fTP[2] == 1.25 and params.fTP[3] == 1.25)
        assert(params.accuracyModifier[1] == 0 and params.accuracyModifier[2] == 30 and params.accuracyModifier[3] == 60)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(decimation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
