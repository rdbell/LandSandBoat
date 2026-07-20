require('scripts/actions/mobskills/drainkiss')

describe('Drainkiss mob skill', function()
    it('uses its magical HP-drain plan and reports a drain message only after damage processing', function()
        local drainkiss = require('scripts/actions/mobskills/drainkiss')
        local magicalMove, processDamage, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message, drain = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123 }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, drainType, amount)
            drain = { drainType, amount }
            return 777
        end

        assert(drainkiss.onMobSkillCheck({}, {}, {}) == 0)
        assert(drainkiss.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        assert(params.baseDamage == 77)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1.25 and params.fTP[3] == 1.5)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.skipMagicBonusDiff == true)
        assert(message == nil and drain == nil)

        xi.mobskills.processDamage = function() return true end
        assert(drainkiss.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = magicalMove, processDamage, drainMove
        assert(message == 777 and drain[1] == xi.mobskills.drainType.HP and drain[2] == 123)
    end)
end)
