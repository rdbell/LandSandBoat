require('scripts/actions/mobskills/aegis_schism')
describe('Aegis Schism mob skill', function()
    it('requests fixed Defense Down and forwards its message', function()
        local move, params, message = xi.mobskills.mobStatusEffectMove, nil, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) params = { ... }; return 77 end
        local skill = { setMsg = function(_, value) message = value end }
        local schism = require('scripts/actions/mobskills/aegis_schism')
        assert(schism.onMobSkillCheck({}, {}, skill) == 0 and schism.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobStatusEffectMove = move
        assert(params[1] == xi.effect.DEFENSE_DOWN and params[2] == 75 and params[3] == 0 and params[4] == 120 and message == 77)
    end)
end)
