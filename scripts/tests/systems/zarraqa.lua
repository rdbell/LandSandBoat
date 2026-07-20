require('scripts/actions/mobskills/zarraqa')
describe('Zarraqa mob skill', function()
    it('uses Fire magical plan with dStat 1 and processed damage', function()
        local skill = require('scripts/actions/mobskills/zarraqa')
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
            return { damage = 75, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 75)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.0 and params.dStatMultiplier == 1)
        assert(params.element == xi.element.FIRE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 75)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 75)
    end)
end)
