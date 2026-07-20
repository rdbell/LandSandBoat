require('scripts/actions/mobskills/double_down')

describe('Double Down mob skill', function()
    it('requires an NM and uses its physical plan after damage processing', function()
        local down = require('scripts/actions/mobskills/double_down')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local nm = { isNM = function() return true end, getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(down.onMobSkillCheck({}, { isNM = function() return false end }, {}) == 1)
        assert(down.onMobSkillCheck({}, nm, {}) == 0)
        assert(down.onMobWeaponSkill(nm, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.attackMultiplier[1] == 2 and params.attackMultiplier[2] == 2 and params.attackMultiplier[3] == 2 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(down.onMobWeaponSkill(nm, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == nm and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
