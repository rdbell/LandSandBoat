require('scripts/actions/mobskills/berserk_bomb_big')
describe('Berserk Bomb Big mob skill', function()
    it('applies Berserk power 50 for 180s', function()
        local skill = require('scripts/actions/mobskills/berserk_bomb_big')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 50
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BERSERK)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == xi.effect.BERSERK and buff[2] == 50 and buff[4] == 180)
        assert(message == 50)
    end)
end)
