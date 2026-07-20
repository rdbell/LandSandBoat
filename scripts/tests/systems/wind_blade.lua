require('scripts/actions/mobskills/wind_blade')
describe('Wind Blade mob skill', function()
    it('uses its Wind magical plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/wind_blade')
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
            return { damage = 200, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.baseDamage == 52 and params.fTP[1] == 8.0)
        assert(params.element == xi.element.WIND and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 200 and damage[4] == xi.damageType.WIND)
    end)
end)
