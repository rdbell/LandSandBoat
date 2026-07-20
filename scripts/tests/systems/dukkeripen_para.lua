require('scripts/actions/mobskills/dukkeripen_para')

describe('Dukkeripen Para mob skill', function()
    it('allows Corsair only and reports application or miss for Paralysis', function()
        local para = require('scripts/actions/mobskills/dukkeripen_para')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local request, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) request = { effect, power, tick, duration }; return true end
        assert(para.onMobSkillCheck({}, { getMainJob = function() return xi.job.COR end }, {}) == 0)
        assert(para.onMobSkillCheck({}, { getMainJob = function() return xi.job.WAR end }, {}) == 1)
        assert(para.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        assert(request[1] == xi.effect.PARALYSIS and request[2] == 20 and request[3] == 0 and request[4] == 120 and message == xi.msg.basic.SKILL_ENFEEB_IS)
        xi.mobskills.mobStatusEffectMove = function() return false end
        assert(para.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(message == xi.msg.basic.SKILL_MISS)
    end)
end)
