describe('Impale Bind mob skill', function()
    it('sets the Bind result message and returns Bind without damage', function()
        local impale = require('scripts/actions/mobskills/impale_bind')
        local status = xi.mobskills.mobStatusEffectMove
        local message, args
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) args = { ... }; return 777 end
        assert(impale.onMobSkillCheck({}, {}, skill) == 0 and impale.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.BIND)
        xi.mobskills.mobStatusEffectMove = status
        assert(message == 777 and args[1] == xi.effect.BIND and args[2] == 1 and args[3] == 0 and args[4] == 15)
    end)
end)
