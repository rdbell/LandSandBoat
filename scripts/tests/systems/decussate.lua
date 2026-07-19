require('scripts/actions/mobskills/decussate')

describe('Decussate mob skill', function()
    it('unlocks at 20 percent HP and uses its physical plan', function()
        local params, damage = nil, nil
        local hpp = 21
        local mob = { getHPP = function() return hpp end, getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        local decussate = require('scripts/actions/mobskills/decussate')
        assert(decussate.onMobSkillCheck({}, mob, {}) == 1)
        hpp = 20
        assert(decussate.onMobSkillCheck({}, mob, {}) == 0)
        assert(decussate.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(decussate.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
