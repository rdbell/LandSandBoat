require('scripts/actions/mobskills/rhino_guard')
describe('Rhino Guard mob skill', function()
    it('applies Evasion Boost with TP-scaled duration', function()
        local guard = require('scripts/actions/mobskills/rhino_guard')
        local buff = xi.mobskills.mobBuffMove
        local params, message, tp = nil, nil, 1000
        local mob = {}
        local skill = {
            getTP = function() return tp end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(guard.onMobSkillCheck({}, mob, skill) == 0)
        assert(guard.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.EVASION_BOOST)
        assert(params[2] == xi.effect.EVASION_BOOST and params[3] == 25 and params[5] == 180)
        tp = 2000
        guard.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobBuffMove = buff
        assert(params[5] == 480 and message == 456)
    end)
end)
