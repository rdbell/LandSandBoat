describe('Fossilizing Breath mob skill', function()
    it('allows use, requests Petrification, and forwards its message', function()
        local breath = require('scripts/actions/mobskills/fossilizing_breath')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) args = { ... }; return 777 end
        assert(breath.onMobSkillCheck(target, mob, skill) == 0 and breath.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PETRIFICATION)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[1] == mob and args[2] == target and args[3] == xi.effect.PETRIFICATION and args[4] == 1 and args[5] == 0 and args[6] == 60 and message == 777)
    end)
end)
