require('scripts/actions/mobskills/royal_savior')
describe('Royal Savior mob skill', function()
    it('applies Protect for non-trust and defense suite for trusts', function()
        local savior = require('scripts/actions/mobskills/royal_savior')
        local buff = xi.mobskills.mobBuffMove
        local isTrust, params, effects, message, tp = false, nil, {}, nil, 1000
        local mob = {
            isTrust = function() return isTrust end,
            getMaxHP = function() return 500 end,
            addStatusEffect = function(_, effect, opts) effects[#effects+1] = { effect, opts } end,
            setTP = function(_, v) tp = v end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) params = { ... }; return 456 end
        assert(savior.onMobSkillCheck({}, mob, skill) == 0)
        assert(savior.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.PROTECT)
        assert(params[2] == xi.effect.PROTECT and params[3] == 175 and params[5] == 300)
        isTrust, effects, tp = true, {}, 1000
        assert(savior.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buff
        assert(params[2] == xi.effect.DEFENSE_BOOST and params[3] == 50 and params[5] == 60)
        assert(effects[1][1] == xi.effect.PALISADE and effects[1][2].power == 30)
        assert(effects[2][1] == xi.effect.STONESKIN and effects[2][2].power == 50)
        assert(tp == 0 and message == 456)
    end)
end)
