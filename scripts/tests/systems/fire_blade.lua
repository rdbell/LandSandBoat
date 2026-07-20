describe('Fire Blade mob skill', function()
    it('allows use, applies Enfire, and forwards the buff message', function()
        local fireBlade = require('scripts/actions/mobskills/fire_blade')
        local buffMove = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            params = { target, effect, power, tick, duration }
            return 777
        end
        assert(fireBlade.onMobSkillCheck(nil, mob, skill) == 0)
        assert(fireBlade.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.ENFIRE)
        xi.mobskills.mobBuffMove = buffMove
        assert(params[1] == mob and params[2] == xi.effect.ENFIRE and params[3] == 65 and params[4] == 0 and params[5] == 30)
        assert(message == 777)
    end)
end)
