require('scripts/actions/mobskills/meikyo_shisui')

describe('Meikyo Shisui mob skill', function()
    it('allows use, applies its buff, grants TP, and initializes its counter', function()
        local meikyoShisui = require('scripts/actions/mobskills/meikyo_shisui')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, tp, localVar, message = nil, nil, nil, nil
        local mob = {
            addTP = function(_, value) tp = value end,
            setLocalVar = function(_, name, value) localVar = { name, value } end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { target, effect, power, tick, duration }
            return 999
        end

        assert(meikyoShisui.onMobSkillCheck(nil, mob, skill) == 0)
        assert(meikyoShisui.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.MEIKYO_SHISUI)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.MEIKYO_SHISUI)
        assert(buff[3] == 1 and buff[4] == 0 and buff[5] == 30)
        assert(message == xi.msg.basic.USES and tp == 3000)
        assert(localVar[1] == '[MeikyoShisui]MobSkillCount' and localVar[2] == 3)
    end)
end)
