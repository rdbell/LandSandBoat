require('scripts/actions/mobskills/deadeye')

describe('Deadeye mob skill', function()
    it('is always available', function()
        local deadeye = require('scripts/actions/mobskills/deadeye')

        assert(deadeye.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('prefers Defense Down when both enfeebles are reported', function()
        local message, requests = nil, {}
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(requests, { effect, power, tick, duration })
            return xi.msg.basic.SKILL_ENFEEB_IS
        end
        local skill = { setMsg = function(_, value) message = value end }
        local deadeye = require('scripts/actions/mobskills/deadeye')

        assert(deadeye.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)
        assert(requests[1][1] == xi.effect.DEFENSE_DOWN and requests[2][1] == xi.effect.MAGIC_DEF_DOWN)
        assert(requests[1][2] == 50 and requests[1][3] == 0 and requests[1][4] == 120)
        assert(requests[2][2] == 50 and requests[2][3] == 0 and requests[2][4] == 120)
    end)

    it('reports magic defense down when only it succeeds, otherwise misses', function()
        local responses = { 0, xi.msg.basic.SKILL_ENFEEB_IS }
        local message = nil
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function()
            local response = table.remove(responses, 1)
            return response
        end
        local skill = { setMsg = function(_, value) message = value end }
        local deadeye = require('scripts/actions/mobskills/deadeye')

        assert(deadeye.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.MAGIC_DEF_DOWN)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)

        responses = { 0, 0 }
        assert(deadeye.onMobWeaponSkill({}, {}, skill, {}) == nil)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(message == xi.msg.basic.SKILL_MISS)
    end)
end)
