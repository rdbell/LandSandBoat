require('scripts/actions/mobskills/marrow_drain')

describe('Marrow Drain mob skill', function()
    it('uses its magical MP-drain request and only drains after processing', function()
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drain, message = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123 }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, drainType, amount)
            drain = { drainType, amount }
            return 77
        end
        local skillObject = require('scripts/actions/mobskills/marrow_drain')
        assert(skillObject.onMobSkillCheck(target, mob, skill) == 0 and skillObject.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 73 and params.additiveDamage[1] == 0 and params.additiveDamage[2] == 5 and params.additiveDamage[3] == 10)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.element == xi.element.NONE)
        assert(params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipDamageAdjustment == true and params.skipMagicBonusDiff == true and params.skipStoneSkin == true and drain == nil and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skillObject.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove
        assert(drain[1] == xi.mobskills.drainType.MP and drain[2] == 123 and message == 77)
    end)
end)
