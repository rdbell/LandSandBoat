require('scripts/actions/mobskills/void_of_repentance')
describe('Void Of Repentance mob skill', function()
    it('applies Terror for 30s', function()
        local skill = require('scripts/actions/mobskills/void_of_repentance')
        local msg = nil
        local orig = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            assert(e == xi.effect.TERROR and power == 1 and dur == 30)
            return 242
        end
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.TERROR)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = orig
    end)
end)
