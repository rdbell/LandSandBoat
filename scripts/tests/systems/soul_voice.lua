require('scripts/actions/mobskills/soul_voice')
describe('Soul Voice mob skill', function()
    it('applies Soul Voice buff and USES message', function()
        local voice = require('scripts/actions/mobskills/soul_voice')
        local buff = xi.mobskills.mobBuffMove
        local params, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 999 end
        assert(voice.onMobSkillCheck({}, {}, skill) == 0)
        assert(voice.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SOUL_VOICE)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.SOUL_VOICE and params[3] == 1 and params[5] == 180)
        assert(message == xi.msg.basic.USES)
    end)
end)
