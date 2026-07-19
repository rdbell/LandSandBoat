require('scripts/actions/mobskills/frightful_roar')

describe('Frightful Roar mob skill', function()
    it('always allows use and applies fixed Defense Down parameters', function()
        local frightfulRoar = require('scripts/actions/mobskills/frightful_roar')
        local oldMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) args = { ... }; return 321 end

        assert(frightfulRoar.onMobSkillCheck({}, {}, skill) == 0)
        assert(frightfulRoar.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobStatusEffectMove = oldMove

        assert(args[3] == xi.effect.DEFENSE_DOWN and args[4] == 77 and args[5] == 0 and args[6] == 180)
        assert(message == 321)
    end)
end)
