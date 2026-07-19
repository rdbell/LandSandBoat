require('scripts/actions/mobskills/crimson_howl')

describe('Crimson Howl mob skill', function()
    it('applies Warcry and forwards the buff message', function()
        local crimsonHowl = require('scripts/actions/mobskills/crimson_howl')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 123
        end

        assert(crimsonHowl.onMobSkillCheck(nil, mob, skill) == 0)
        assert(crimsonHowl.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.WARCRY)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.WARCRY)
        assert(buff[3] == 25 and buff[4] == 0 and buff[5] == 180)
        assert(message == 123)
    end)
end)
