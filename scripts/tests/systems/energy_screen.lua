require('scripts/actions/mobskills/energy_screen')

describe('Energy Screen mob skill', function()
    it('applies Physical Shield and forwards the buff message', function()
        local screen = require('scripts/actions/mobskills/energy_screen')
        local buffMove = xi.mobskills.mobBuffMove
        local request, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(source, effect, power, tick, duration)
            request = { source, effect, power, tick, duration }
            return 777
        end

        assert(screen.onMobSkillCheck({}, mob, skill) == 0)
        assert(screen.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PHYSICAL_SHIELD)
        xi.mobskills.mobBuffMove = buffMove
        assert(request[1] == mob and request[2] == xi.effect.PHYSICAL_SHIELD and request[3] == 1 and request[4] == 0 and request[5] == 60 and message == 777)
    end)
end)
