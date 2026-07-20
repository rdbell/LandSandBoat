require('scripts/actions/mobskills/tabula_rasa')
describe('Tabula Rasa mob skill', function()
    it('sets USES and returns TABULA_RASA', function()
        local skill = require('scripts/actions/mobskills/tabula_rasa')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.TABULA_RASA)
        assert(msg == xi.msg.basic.USES)
    end)
end)
