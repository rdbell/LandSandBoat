require('scripts/actions/mobskills/abyssal_drain')

describe('Abyssal Drain mob skill', function()
    it('retains its blocked check and drains HP only after processed magical damage', function()
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drain, message = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, drainType, amount, attackType, damageType)
            drain = { drainType, amount, attackType, damageType }
            return 77
        end
        local skillObject = require('scripts/actions/mobskills/abyssal_drain')
        assert(skillObject.onMobSkillCheck(target, mob, skill) == 1 and skillObject.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and params.skipMagicBonusDiff == true and drain == nil and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skillObject.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove
        assert(drain[1] == xi.mobskills.drainType.HP and drain[2] == 123 and drain[3] == xi.attackType.MAGICAL and drain[4] == xi.damageType.NONE and message == 77)
    end)
end)
