require('scripts/actions/mobskills/roar')
describe('Roar mob skill', function()
    it('applies fixed Paralysis status', function()
        local roar = require('scripts/actions/mobskills/roar')
        local status = xi.mobskills.mobStatusEffectMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(roar.onMobSkillCheck({}, {}, skill) == 0)
        assert(roar.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.PARALYSIS and params[4] == 50 and params[6] == 120 and message == 456)
    end)
end)
