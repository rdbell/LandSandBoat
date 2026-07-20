require('scripts/actions/mobskills/mix_guard_drink')
describe('Mix Guard Drink mob skill', function()
    it('applies Protect and Shell with self vs target messages', function()
        local skill = require('scripts/actions/mobskills/mix_guard_drink')
        local msg, buffs = nil, {}
        local mob = { getID = function() return 1 end }
        local target = {
            getID = function() return 1 end,
            addStatusEffect = function(_, e, opts) buffs[#buffs + 1] = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.PROTECT)
        assert(msg == 194)
        assert(buffs[1][1] == xi.effect.PROTECT and buffs[1][2] == 220 and buffs[1][3] == 300)
        assert(buffs[2][1] == xi.effect.SHELL and buffs[2][2] == 2930 and buffs[2][3] == 300)
        buffs, msg = {}, nil
        target.getID = function() return 2 end
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(msg == 280 and #buffs == 2)
    end)
end)
