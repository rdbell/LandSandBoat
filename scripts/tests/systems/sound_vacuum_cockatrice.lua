require('scripts/actions/mobskills/sound_vacuum_cockatrice')
describe('Sound Vacuum Cockatrice mob skill', function()
    it('applies Silence for 120 seconds', function()
        local vac = require('scripts/actions/mobskills/sound_vacuum_cockatrice')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(vac.onMobSkillCheck({}, {}, skill) == 0)
        assert(vac.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SILENCE)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.SILENCE and params[4] == 1 and params[6] == 120 and message == 456)
    end)
end)
