require('scripts/actions/mobskills/dukkeripen_shadow')

describe('Dukkeripen Shadow mob skill', function()
    it('allows Corsair only and applies ten Blink shadows with its buff message', function()
        local shadow = require('scripts/actions/mobskills/dukkeripen_shadow')
        local buffMove = xi.mobskills.mobBuffMove
        local request, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration) request = { effect, power, tick, duration }; return 123 end
        assert(shadow.onMobSkillCheck({}, { getMainJob = function() return xi.job.COR end }, {}) == 0)
        assert(shadow.onMobSkillCheck({}, { getMainJob = function() return xi.job.WAR end }, {}) == 1)
        assert(shadow.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BLINK)
        xi.mobskills.mobBuffMove = buffMove
        assert(request[1] == xi.effect.BLINK and request[2] == 10 and request[3] == 0 and request[4] == 120 and message == 123)
    end)
end)
