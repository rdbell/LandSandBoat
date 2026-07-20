describe('Invincible mob skill', function()
    it('requests a fixed Invincible buff, sets USES, and returns Invincible', function()
        local invincible = require('scripts/actions/mobskills/invincible')
        local buff = xi.mobskills.mobBuffMove
        local call, message
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(...)
            call = { ... }
            return 777
        end

        assert(invincible.onMobSkillCheck({}, mob, skill) == 0 and invincible.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.INVINCIBLE)
        xi.mobskills.mobBuffMove = buff

        assert(call[1] == mob and call[2] == xi.effect.INVINCIBLE and call[3] == 1 and call[4] == 0 and call[5] == 30 and message == xi.msg.basic.USES)
    end)
end)
