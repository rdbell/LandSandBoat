describe('Infrasonics mob skill', function()
    it('sets the Evasion Down result message and returns Evasion Down', function()
        local infrasonics = require('scripts/actions/mobskills/infrasonics')
        local status = xi.mobskills.mobStatusEffectMove
        local message, args
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) args = { ... }; return 777 end
        assert(infrasonics.onMobSkillCheck({}, {}, skill) == 0 and infrasonics.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.EVASION_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(message == 777 and args[1] == xi.effect.EVASION_DOWN and args[2] == 40 and args[3] == 0 and args[4] == 180)
    end)
end)
