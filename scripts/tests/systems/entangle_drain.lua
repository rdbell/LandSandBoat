require('scripts/actions/mobskills/entangle_drain')

describe('Entangle Drain mob skill', function()
    it('uses its physical plan and applies drain then Bind only after processing', function()
        local drain = require('scripts/actions/mobskills/entangle_drain')
        local physicalMove, processDamage, drainMove, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove, xi.mobskills.mobStatusEffectMove
        local params, calls, message = nil, {}, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, kind, damage) table.insert(calls, { 'drain', kind, damage }); return 777 end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) table.insert(calls, { 'bind', effect, power, tick, duration }); return 0 end

        assert(drain.onMobSkillCheck({}, {}, {}) == 0)
        assert(drain.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and #calls == 0 and message == nil)

        xi.mobskills.processDamage = function() return true end
        assert(drain.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, drainMove, statusMove
        assert(calls[1][1] == 'drain' and calls[1][2] == xi.mobskills.drainType.HP and calls[1][3] == 123 and calls[2][1] == 'bind' and calls[2][2] == xi.effect.BIND and calls[2][3] == 1 and calls[2][4] == 0 and calls[2][5] == 60 and message == 777)
    end)
end)
