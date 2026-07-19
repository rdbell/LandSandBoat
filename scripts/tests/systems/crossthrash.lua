require('scripts/actions/mobskills/crossthrash')

describe('Crossthrash mob skill', function()
    it('uses fixed physical parameters and damages and dispels only after processing succeeds', function()
        local crossthrash = require('scripts/actions/mobskills/crossthrash')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage, dispelled = nil, nil, false
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end,
            dispelStatusEffect = function() dispelled = true end,
        }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(crossthrash.onMobSkillCheck(target, mob, skill) == 0)
        assert(crossthrash.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and not dispelled)

        xi.mobskills.processDamage = function() return true end
        assert(crossthrash.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(dispelled)
    end)
end)
