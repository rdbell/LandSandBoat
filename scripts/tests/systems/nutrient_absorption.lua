require('scripts/actions/mobskills/nutrient_absorption')

describe('Nutrient Absorption mob skill', function()
    it('uses its magical HP-drain request and only drains after processing', function()
        local absorb = require('scripts/actions/mobskills/nutrient_absorption')
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drain, message = nil, nil, nil
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
        assert(absorb.onMobSkillCheck({}, {}, skill) == 0 and absorb.onMobWeaponSkill({}, {}, skill, {}) == 123)
        assert(params.baseDamage == 300 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.skipMagicBonusDiff and params.dStatMultiplier == 1)
        assert(drain == nil and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(absorb.onMobWeaponSkill({}, {}, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove
        assert(drain[1] == xi.mobskills.drainType.HP and drain[2] == 123 and message == 77)
    end)
end)
