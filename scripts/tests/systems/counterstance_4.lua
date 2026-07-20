require('scripts/actions/mobskills/counterstance_4')
describe('Counterstance 4 mob skill', function()
    it('applies COUNTERSTANCE buff', function()
        local skill = require('scripts/actions/mobskills/counterstance_4')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        local action = { setCategory = function() end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, action) == xi.effect.COUNTERSTANCE)
        assert(buff[1] == xi.effect.COUNTERSTANCE and buff[2] == 45 and buff[4] == 300)
        assert(msg == xi.msg.basic.NONE)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
