require('scripts/actions/mobskills/wind_shear')
describe('Wind Shear mob skill', function()
    it('uses its scaled Wind magical plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/wind_shear')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.25 and params.fTP[2] == 1.50 and params.fTP[3] == 1.75)
        assert(params.element == xi.element.WIND and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 90)
    end)
end)
