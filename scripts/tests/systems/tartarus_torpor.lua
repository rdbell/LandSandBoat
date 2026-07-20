require('scripts/actions/mobskills/tartarus_torpor')
describe('Tartarus Torpor mob skill', function()
    it('applies MDEF/MEVA down and Sleep with TP-scaled duration', function()
        local skill = require('scripts/actions/mobskills/tartarus_torpor')
        local msg, statuses = nil, {}
        local orig = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            statuses[#statuses+1] = { e, power, tick, dur }
            return 242
        end
        local sk = { getTP = function() return 1000 end, setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SLEEP_I)
        assert(#statuses == 3)
        assert(statuses[1][1] == xi.effect.MAGIC_DEF_DOWN and statuses[1][4] == 60)
        assert(statuses[3][1] == xi.effect.SLEEP_I and msg == 242)
        xi.mobskills.mobStatusEffectMove = orig
    end)
end)
