require('scripts/actions/mobskills/dark_orb')

describe('Dark Orb mob skill', function()
    it('is available only while flying', function()
        local skill = require('scripts/actions/mobskills/dark_orb')

        assert(skill.onMobSkillCheck({}, { getAnimationSub = function() return 5 end }, {}) == 0)
        assert(skill.onMobSkillCheck({}, { getAnimationSub = function() return 4 end }, {}) == 1)
    end)

    it('uses its Dark magical plan and applies processed damage', function()
        local params, damage = nil, nil
        local mob = {
            getMainLvl = function()
                return 50
            end,
        }
        local target = {
            takeDamage = function(_, amount, source, attackType, damageType)
                damage = { amount, source, attackType, damageType }
            end,
        }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return {
                damage = 123,
                attackType = xi.attackType.MAGICAL,
                damageType = xi.damageType.DARK,
            }
        end
        xi.mobskills.processDamage = function()
            return true
        end

        local orb = require('scripts/actions/mobskills/dark_orb')
        assert(orb.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(params.baseDamage == 52)
        assert(params.fTP[1] == 5.5 and params.fTP[2] == 5.5 and params.fTP[3] == 5.5)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
