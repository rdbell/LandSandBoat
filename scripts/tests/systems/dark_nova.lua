require('scripts/actions/mobskills/dark_nova')

describe('Dark Nova mob skill', function()
    it('rejects the magic-immune animation state', function()
        local skill = require('scripts/actions/mobskills/dark_nova')

        assert(skill.onMobSkillCheck({}, { getAnimationSub = function() return 1 end }, {}) == 1)
        assert(skill.onMobSkillCheck({}, { getAnimationSub = function() return 0 end }, {}) == 0)
    end)

    it('uses its Dark-element Fire-damage magical plan and applies processed damage', function()
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
                damageType = xi.damageType.FIRE,
            }
        end
        xi.mobskills.processDamage = function()
            return true
        end

        local nova = require('scripts/actions/mobskills/dark_nova')
        assert(nova.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(params.baseDamage == 52)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
