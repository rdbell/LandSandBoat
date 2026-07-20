require('scripts/actions/mobskills/spore')
describe('Spore mob skill', function()
    it('applies Paralysis status', function()
        local spore = require('scripts/actions/mobskills/spore')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(spore.onMobSkillCheck({}, {}, skill) == 0)
        assert(spore.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 15 and params[6] == 180 and message == 456)
    end)
end)
