require('scripts/actions/mobskills/enervation')

describe('Enervation mob skill', function()
    it('attempts both debuffs and selects its message and return effect by success priority', function()
        local enervation = require('scripts/actions/mobskills/enervation')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local requests, messages = {}, {}
        local skill = { setMsg = function(_, value) table.insert(messages, value) end }

        local outcomes = { xi.msg.basic.SKILL_ENFEEB_IS, xi.msg.basic.SKILL_ENFEEB_IS }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(requests, { effect, power, tick, duration })
            return table.remove(outcomes, 1)
        end
        assert(enervation.onMobSkillCheck({}, {}, skill) == 0)
        assert(enervation.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        assert(requests[1][1] == xi.effect.DEFENSE_DOWN and requests[2][1] == xi.effect.MAGIC_DEF_DOWN and requests[1][2] == 50 and requests[1][3] == 0 and requests[1][4] == 30 and messages[1] == xi.msg.basic.SKILL_ENFEEB_IS)

        outcomes, messages = { 0, xi.msg.basic.SKILL_ENFEEB_IS }, {}
        assert(enervation.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.MAGIC_DEF_DOWN)
        assert(messages[1] == xi.msg.basic.SKILL_ENFEEB_IS)

        outcomes, messages = { 0, 0 }, {}
        assert(enervation.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(messages[1] == xi.msg.basic.SKILL_ENFEEB_IS and messages[2] == xi.msg.basic.SKILL_MISS)
    end)
end)
