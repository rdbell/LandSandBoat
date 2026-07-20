require('scripts/actions/mobskills/stygian_flatus')
describe('Stygian Flatus mob skill', function()
    it('applies Paralysis and forwards status message', function()
        local flatus = require('scripts/actions/mobskills/stygian_flatus')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(flatus.onMobSkillCheck({}, {}, skill) == 0)
        assert(flatus.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 20 and params[6] == 90 and message == 456)
    end)
end)
