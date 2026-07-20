require('scripts/actions/mobskills/saline_coat')
describe('Saline Coat mob skill', function()
    it('applies TP-scaled Magic Def Boost', function()
        local coat = require('scripts/actions/mobskills/saline_coat')
        local buff = xi.mobskills.mobBuffMove
        local params, message, tp = nil, nil, 1000
        local mob = {}
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(coat.onMobSkillCheck({}, mob, skill) == 0)
        assert(coat.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.MAGIC_DEF_BOOST)
        assert(params[2] == xi.effect.MAGIC_DEF_BOOST and params[3] == 100 and params[5] == 60)
        tp = 2000
        coat.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobBuffMove = buff
        assert(params[3] == 110 and message == 456)
    end)
end)
