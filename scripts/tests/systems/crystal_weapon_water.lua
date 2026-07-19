require('scripts/actions/mobskills/crystal_weapon_water')

describe('Crystal Weapon Water mob skill', function()
    it('uses fixed Water magical parameters and damages only after processing succeeds', function()
        local crystalWeaponWater = require('scripts/actions/mobskills/crystal_weapon_water')
        local originalMagicalMove = xi.mobskills.mobMagicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end

        assert(crystalWeaponWater.onMobSkillCheck(target, mob, skill) == 0)
        assert(crystalWeaponWater.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.WATER and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.WATER and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(crystalWeaponWater.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobMagicalMove = originalMagicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WATER)
    end)
end)
