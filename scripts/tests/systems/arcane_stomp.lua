require('scripts/actions/mobskills/arcane_stomp')
describe('Arcane Stomp mob skill', function()
    it('applies Magic Shield power 2 for 300s', function()
        local skill = require('scripts/actions/mobskills/arcane_stomp')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 77
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.MAGIC_SHIELD)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == xi.effect.MAGIC_SHIELD and buff[2] == 2 and buff[4] == 300)
        assert(message == 77)
    end)
end)
