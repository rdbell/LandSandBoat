require('scripts/actions/mobskills/proboscis')

describe('Proboscis mob skill', function()
    it('drains MP and dispels one beneficial after processing', function()
        local proboscis = require('scripts/actions/mobskills/proboscis')
        local move, process, drain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drainArgs, message, dispelled = nil, nil, nil, false
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            dispelStatusEffect = function()
                dispelled = true
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...)
            drainArgs = { ... }
            return 456
        end
        xi.mobskills.processDamage = function() return false end
        assert(proboscis.onMobSkillCheck(target, mob, skill) == 0 and proboscis.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 48 and params.fTP[1] == 4 and params.skipDamageAdjustment)
        assert(drainArgs == nil and not dispelled and message == nil)
        xi.mobskills.processDamage = function() return true end
        proboscis.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(drainArgs[3] == xi.mobskills.drainType.MP and drainArgs[4] == 123 and message == 456 and dispelled)
    end)
end)
