require('scripts/actions/mobskills/mix_insomniant')
describe('Mix Insomniant mob skill', function()
    it('applies NEGATE_SLEEP with message 194', function()
        local skill = require('scripts/actions/mobskills/mix_insomniant')
        local msg, added, has = nil, nil, false
        local target = {
            hasStatusEffect = function(_, e) return has and e == xi.effect.NEGATE_SLEEP end,
            addStatusEffect = function(_, e, opts) added = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.NEGATE_SLEEP)
        assert(msg == 194 and added[1] == xi.effect.NEGATE_SLEEP and added[2] == 10 and added[3] == 60)
        has, added = true, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == 194 and added == nil)
    end)
end)
