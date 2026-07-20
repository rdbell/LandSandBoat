require('scripts/actions/mobskills/soul_accretion')
describe('Soul Accretion mob skill', function()
    it('steals status and sets drained or no-effect message', function()
        local acc = require('scripts/actions/mobskills/soul_accretion')
        local stole, message = 0, nil
        local mob = {
            stealStatusEffect = function(_, t, flags)
                assert(bit.band(flags, xi.effectFlag.DISPELABLE) ~= 0)
                assert(bit.band(flags, xi.effectFlag.FOOD) ~= 0)
                return stole
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(acc.onMobSkillCheck({}, mob, skill) == 0)
        assert(acc.onMobWeaponSkill(mob, {}, skill, {}) == 1)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        stole = 5
        assert(acc.onMobWeaponSkill(mob, {}, skill, {}) == 1)
        assert(message == xi.msg.basic.EFFECT_DRAINED)
    end)
end)
