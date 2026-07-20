require('scripts/actions/mobskills/whistle')
describe('Whistle mob skill', function()
    it('applies AGI Boost and returns the effect', function()
        local skill = require('scripts/actions/mobskills/whistle')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 77
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.AGI_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[2] == xi.effect.AGI_BOOST and buff[3] == 10 and buff[4] == 3 and buff[5] == 210)
        assert(message == 77)
    end)
end)
