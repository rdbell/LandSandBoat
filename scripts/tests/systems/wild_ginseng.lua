require('scripts/actions/mobskills/wild_ginseng')
describe('Wild Ginseng mob skill', function()
    it('applies Haste Protect Shell Regen Blink and messages Blink', function()
        local skill = require('scripts/actions/mobskills/wild_ginseng')
        local buffMove = xi.mobskills.mobBuffMove
        local buffs, message = {}, nil
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            table.insert(buffs, { target, effect, power, tick, duration })
            return 100 + #buffs
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BLINK)
        xi.mobskills.mobBuffMove = buffMove
        assert(#buffs == 5)
        assert(buffs[1][2] == xi.effect.HASTE and buffs[1][3] == 2000 and buffs[1][5] == 180)
        assert(buffs[2][2] == xi.effect.PROTECT and buffs[2][3] == 60)
        assert(buffs[3][2] == xi.effect.SHELL and buffs[3][3] == 1750)
        assert(buffs[4][2] == xi.effect.REGEN and buffs[4][3] == 30)
        assert(buffs[5][2] == xi.effect.BLINK and buffs[5][3] == 3 and buffs[5][5] == 180)
        assert(message == 105)
    end)
end)
