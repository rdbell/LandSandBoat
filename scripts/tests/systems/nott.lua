require('scripts/actions/mobskills/nott')
describe('Nott mob skill', function()
    it('readies skill and restores HP/MP by TP factors', function()
        local skill = require('scripts/actions/mobskills/nott')
        local msg, basic, cat, hp, mp = nil, nil, nil, 0, 0
        local mob = {
            messageBasic = function(_, m) basic = m end,
            getMaxHP = function() return 1000 end,
            getMaxMP = function() return 500 end,
            addHP = function(_, v) hp = v end,
            addMP = function(_, v) mp = v end,
        }
        local sk = { getTP = function() return 1000 end, setMsg = function(_, m) msg = m end }
        local action = { setCategory = function(_, c) cat = c end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(basic == xi.msg.basic.READIES_SKILL)
        assert(skill.onMobWeaponSkill(mob, {}, sk, action) == 220)
        assert(hp == 220 and mp == 75 and msg == xi.msg.basic.SKILL_RECOVERS_HP)
        assert(cat == xi.action.category.WEAPONSKILL_FINISH)
    end)
end)
