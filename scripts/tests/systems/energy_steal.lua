require('scripts/actions/mobskills/energy_steal')

describe('Energy Steal mob skill', function()
    it('uses its MP-drain magical plan and sets its drain message only after processing', function()
        local steal = require('scripts/actions/mobskills/energy_steal')
        local magicalMove, processDamage, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, calls, message = nil, 0, nil
        local mob = { getMainLvl = function() return 75 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123 } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, kind, damage) calls = calls + 1; assert(kind == xi.mobskills.drainType.MP and damage == 123); return 777 end

        assert(steal.onMobSkillCheck({}, {}, {}) == 0)
        assert(steal.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1 and params.fTP[2] == 2.10 and params.fTP[3] == 3.4)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipDamageAdjustment and params.skipMagicBonusDiff and params.skipStoneSkin and calls == 0 and message == nil)

        xi.mobskills.processDamage = function() return true end
        assert(steal.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = magicalMove, processDamage, drainMove
        assert(calls == 1 and message == 777)
    end)
end)
