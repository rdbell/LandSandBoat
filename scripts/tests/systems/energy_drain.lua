require('scripts/actions/mobskills/energy_drain')

describe('Energy Drain mob skill', function()
    it('uses its MP-drain magical plan and sets its drain message only after processing', function()
        local drain = require('scripts/actions/mobskills/energy_drain')
        local magicalMove, processDamage, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, calls, messages = nil, 0, {}
        local mob = { getMainLvl = function() return 75 end }
        local skill = { setMsg = function(_, value) table.insert(messages, value) end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123 } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, kind, damage) calls = calls + 1; assert(kind == xi.mobskills.drainType.MP and damage == 123); return 777 end

        assert(drain.onMobSkillCheck({}, {}, {}) == 0)
        assert(drain.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1.25 and params.fTP[2] == 2.50 and params.fTP[3] == 4.125)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipDamageAdjustment and params.skipMagicBonusDiff and params.skipStoneSkin and calls == 0 and #messages == 0)

        xi.mobskills.processDamage = function() return true end
        assert(drain.onMobWeaponSkill(mob, {}, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = magicalMove, processDamage, drainMove
        assert(calls == 1 and messages[1] == 777)
    end)
end)
