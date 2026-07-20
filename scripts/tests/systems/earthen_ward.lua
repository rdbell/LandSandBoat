require('scripts/actions/mobskills/earthen_ward')

describe('Earthen Ward mob skill', function()
    it('uses Titan Stoneskin parameters and forwards the buff message', function()
        local ward = require('scripts/actions/mobskills/earthen_ward')
        local buffMove = xi.mobskills.mobBuffMove
        local request, message = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            request = { effect, power, tick, duration }
            return 123
        end

        assert(ward.onMobSkillCheck({}, mob, skill) == 0)
        assert(ward.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.STONESKIN)
        xi.mobskills.mobBuffMove = buffMove

        assert(request[1] == xi.effect.STONESKIN and request[2] == 150 and request[3] == 0 and request[4] == 180)
        assert(message == 123)
    end)
end)
