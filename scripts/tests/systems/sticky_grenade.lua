require('scripts/actions/mobskills/sticky_grenade')
describe('Sticky Grenade mob skill', function()
    it('applies WEIGHT power 50 for 60s', function()
        local skill = require('scripts/actions/mobskills/sticky_grenade')
        local msg = nil
        local orig = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(mob, target, e, power, tick, dur)
            assert(e == xi.effect.WEIGHT and power == 50 and dur == 60)
            return 242
        end
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.WEIGHT)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = orig
    end)
end)
