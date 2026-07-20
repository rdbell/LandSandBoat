require('scripts/actions/mobskills/chainspell')
describe('Chainspell mob skill', function()
    it('applies CHAINSPELL buff and USES message', function()
        local skill = require('scripts/actions/mobskills/chainspell')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.CHAINSPELL)
        assert(buff[1] == xi.effect.CHAINSPELL and buff[2] == 1 and buff[4] == 60)
        assert(msg == xi.msg.basic.USES)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
