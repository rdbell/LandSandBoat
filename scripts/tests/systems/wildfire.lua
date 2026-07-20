require('scripts/actions/mobskills/wildfire')
describe('Wildfire mob skill', function()
    it('uses its Fire magical plan with AGI/INT dStat and processed damage', function()
        local skill = require('scripts/actions/mobskills/wildfire')
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
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 5.5)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.dStatMultiplier == 2 and params.dStatAttackerMod == xi.mod.AGI and params.dStatDefenderMod == xi.mod.INT)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
