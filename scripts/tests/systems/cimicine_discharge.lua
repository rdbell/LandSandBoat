require('scripts/actions/mobskills/cimicine_discharge')
describe('Cimicine Discharge mob skill', function()
    it('applies self Haste when missing and Slow with shared duration', function()
        local skill = require('scripts/actions/mobskills/cimicine_discharge')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local haste, slow, msg = nil, nil, nil
        local origRandom = math.random
        math.random = function(a, b)
            if a == 60 and b == 180 then return 90 end
            return origRandom(a, b)
        end
        local mob = {
            hasStatusEffect = function(_, e) return false end,
            addStatusEffect = function(_, e, opts) haste = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            slow = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.SLOW)
        assert(haste[1] == xi.effect.HASTE and haste[2] == 1500 and haste[3] == 90)
        assert(slow[1] == xi.effect.SLOW and slow[2] == 1950 and slow[4] == 90)
        assert(msg == 242)
        math.random = origRandom
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
