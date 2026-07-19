require('scripts/actions/mobskills/cross_attack')

describe('Cross Attack mob skill', function()
    it('uses fixed physical parameters and damages only after processing succeeds', function()
        local crossAttack = require('scripts/actions/mobskills/cross_attack')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end

        assert(crossAttack.onMobSkillCheck(target, mob, skill) == 0)
        assert(crossAttack.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 2)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.HAND_TO_HAND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        assert(params.attackMultiplier[1] == 1.5 and params.attackMultiplier[2] == 1.5 and params.attackMultiplier[3] == 1.5)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(crossAttack.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.HAND_TO_HAND)
    end)
end)
