require('scripts/actions/mobskills/shiko_no_mitate')
describe('Shiko no Mitate mob skill', function()
    it('applies Defense Boost and trust extras for Gessho', function()
        local mitate = require('scripts/actions/mobskills/shiko_no_mitate')
        local buff = xi.mobskills.mobBuffMove
        local params, effects, message, isTrust = nil, {}, nil, false
        local mob = {
            getObjType = function() return isTrust and xi.objType.TRUST or xi.objType.MOB end,
            addStatusEffect = function(_, effect, opts) effects[#effects+1] = { effect, opts } end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(mitate.onMobSkillCheck({}, mob, skill) == 0)
        assert(mitate.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        assert(params[2] == xi.effect.DEFENSE_BOOST and params[3] == 15 and params[5] == 300 and #effects == 0)
        isTrust, effects = true, {}
        assert(mitate.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(effects[1][1] == xi.effect.ISSEKIGAN and effects[1][2].power == 25)
        assert(effects[2][1] == xi.effect.STONESKIN and effects[2][2].power == 300 and message == 456)
    end)
end)
