require('scripts/actions/mobskills/sound_blast')
describe('Sound Blast mob skill', function()
    it('applies INT Down status', function()
        local blast = require('scripts/actions/mobskills/sound_blast')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(blast.onMobSkillCheck({}, {}, skill) == 0)
        assert(blast.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.INT_DOWN)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.INT_DOWN and params[4] == 10 and params[5] == 3 and params[6] == 180 and message == 456)
    end)
end)
