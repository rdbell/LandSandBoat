require('scripts/actions/mobskills/crystal_weapon_wind')

describe('Crystal Weapon Wind mob skill', function()
    it('uses fixed Wind magical parameters and damages only after processing succeeds', function()
        local crystalWeaponWind = require('scripts/actions/mobskills/crystal_weapon_wind')
        local originalMagicalMove = xi.mobskills.mobMagicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end

        assert(crystalWeaponWind.onMobSkillCheck(target, mob, skill) == 0)
        assert(crystalWeaponWind.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.WIND and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(crystalWeaponWind.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobMagicalMove = originalMagicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
    end)
end)
