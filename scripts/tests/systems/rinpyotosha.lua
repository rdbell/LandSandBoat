require('scripts/actions/mobskills/rinpyotosha')
describe('Rinpyotosha mob skill', function()
    it('applies Warcry to the skill target', function()
        local skillObj = require('scripts/actions/mobskills/rinpyotosha')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(skillObj.onMobSkillCheck(target, {}, skill) == 0)
        assert(skillObj.onMobWeaponSkill({}, target, skill, {}) == xi.effect.WARCRY)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == target and params[2] == xi.effect.WARCRY and params[3] == 25 and params[5] == 180)
        assert(message == 456)
    end)
end)
