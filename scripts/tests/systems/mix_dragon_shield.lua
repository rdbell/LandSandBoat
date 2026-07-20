require('scripts/actions/mobskills/mix_dragon_shield')
describe('Mix Dragon Shield mob skill', function()
    it('applies MAGIC_DEF_BOOST with self vs target messages', function()
        local skill = require('scripts/actions/mobskills/mix_dragon_shield')
        local msg, added, has = nil, nil, false
        local mob = { getID = function() return 1 end }
        local target = {
            getID = function() return 1 end,
            hasStatusEffect = function(_, e) return has and e == xi.effect.MAGIC_DEF_BOOST end,
            addStatusEffect = function(_, e, opts) added = { e, opts.power, opts.duration } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.MAGIC_DEF_BOOST)
        assert(msg == 194 and added[1] == xi.effect.MAGIC_DEF_BOOST and added[2] == 10 and added[3] == 60)
        has, added = true, nil
        target.getID = function() return 2 end
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(msg == 280 and added == nil)
    end)
end)
