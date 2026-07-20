require('scripts/actions/mobskills/parry')
describe('Parry mob skill', function()
    it('applies DEFENSE_BOOST power 25 with TP-scaled duration', function()
        local skill = require('scripts/actions/mobskills/parry')
        local msg, buff = nil, nil
        local orig = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(mob, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 194
        end
        local sk = { getTP = function() return 1000 end, setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.DEFENSE_BOOST)
        assert(buff[1] == xi.effect.DEFENSE_BOOST and buff[2] == 25 and buff[4] == 600 and msg == 194)
        xi.mobskills.mobBuffMove = orig
    end)
end)
