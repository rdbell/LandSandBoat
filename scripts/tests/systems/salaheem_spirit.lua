require('scripts/actions/mobskills/salaheem_spirit')
describe('Salaheem Spirit mob skill', function()
    it('applies all primary stat boosts with TP-scaled duration', function()
        local skill = require('scripts/actions/mobskills/salaheem_spirit')
        local msg, buffs, deleted = nil, {}, {}
        local mob = {
            getMainLvl = function() return 40 end,
            getTP = function() return 1000 end,
        }
        local target = {
            delStatusEffect = function(_, e) deleted[#deleted+1] = e end,
            addStatusEffect = function(_, e, opts) buffs[#buffs+1] = { e, opts.power, opts.duration, opts.tick } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 0)
        assert(#deleted == 7 and #buffs == 7)
        assert(buffs[1][1] == xi.effect.STR_BOOST and buffs[1][2] == 10 and buffs[1][3] == 155 and buffs[1][4] == 10)
        assert(msg == xi.msg.basic.STATUS_BOOST_2)
    end)
end)
