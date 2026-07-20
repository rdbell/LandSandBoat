require('scripts/actions/mobskills/mix_life_water')
describe('Mix Life Water mob skill', function()
    it('applies REGEN with self vs target messages', function()
        local skill = require('scripts/actions/mobskills/mix_life_water')
        local msg, added, has = nil, nil, false
        local mob = { getID = function() return 1 end }
        local target = {
            getID = function() return 1 end,
            hasStatusEffect = function(_, e) return has and e == xi.effect.REGEN end,
            addStatusEffect = function(_, e, opts) added = { e, opts.power, opts.duration, opts.tick } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.REGEN)
        assert(msg == 194 and added[1] == xi.effect.REGEN and added[2] == 20 and added[3] == 60 and added[4] == 3)
        has, added = true, nil
        target.getID = function() return 2 end
        local ret = skill.onMobWeaponSkill(mob, target, sk, {})
        assert(msg == 280 and added == nil and (ret == 0 or ret == nil))
    end)
end)
