require('scripts/actions/mobskills/dark_harvest')

describe('Dark Harvest mob skill', function()
    it('is always available', function()
        local skill = require('scripts/actions/mobskills/dark_harvest')

        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('uses its Int-based Dark magical plan and applies processed damage', function()
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

        local harvest = require('scripts/actions/mobskills/dark_harvest')
        assert(harvest.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(params.baseDamage == 52)
        assert(params.fTP[1] == 1 and params.fTP[2] == 2 and params.fTP[3] == 2.5)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL)
        assert(params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.INT and params.dStatDefenderMod == xi.mod.INT)
        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
