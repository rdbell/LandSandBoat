require('scripts/actions/mobskills/mix_samsons_strength')
describe('Mix Samsons Strength mob skill', function()
    it('applies primary stat boosts with self vs target messages', function()
        local skill = require('scripts/actions/mobskills/mix_samsons_strength')
        local msg, buffs = nil, {}
        local mob = { getID = function() return 1 end }
        local target = {
            getID = function() return 1 end,
            hasStatusEffect = function() return false end,
            addStatusEffect = function(_, e, opts) buffs[#buffs + 1] = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.VIT_BOOST_II)
        assert(msg == 762 and #buffs == 7)
        assert(buffs[1][1] == xi.effect.STR_BOOST and buffs[1][2] == 10 and buffs[1][3] == 60)
        buffs, msg = {}, nil
        target.getID = function() return 2 end
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(msg == 365 and #buffs == 7)
    end)
end)
