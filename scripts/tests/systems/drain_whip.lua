require('scripts/actions/mobskills/drain_whip')

describe('Drain Whip mob skill', function()
    it('plans HP, MP, and TP drains and reports a drain message only after damage processing', function()
        local drainWhip = require('scripts/actions/mobskills/drain_whip')
        local magicalMove, processDamage, drainMove, random = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove, math.random
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

        assert(drainWhip.onMobSkillCheck({}, {}, {}) == 0)

        math.random = function(low, high)
            assert(low == xi.mobskills.drainType.HP and high == xi.mobskills.drainType.TP)
            return xi.mobskills.drainType.HP
        end
        assert(drainWhip.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        assert(params.baseDamage == 77)
        assert(params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and params.skipMagicBonusDiff == true)
        assert(params.skipDamageAdjustment == nil and params.skipStoneSkin == nil)
        assert(message == nil and drain == nil)

        xi.mobskills.processDamage = function() return true end
        math.random = function() return xi.mobskills.drainType.MP end
        assert(drainWhip.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        assert(params.skipDamageAdjustment == true and params.skipMagicBonusDiff == true and params.skipStoneSkin == true)
        assert(message == 777 and drain[1] == xi.mobskills.drainType.MP and drain[2] == 123)

        math.random = function() return xi.mobskills.drainType.TP end
        assert(drainWhip.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove, math.random = magicalMove, processDamage, drainMove, random
        assert(params.skipDamageAdjustment == true and params.skipMagicBonusDiff == true and params.skipStoneSkin == true)
        assert(message == 777 and drain[1] == xi.mobskills.drainType.TP and drain[2] == 123)
    end)
end)
