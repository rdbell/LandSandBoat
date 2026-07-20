require('scripts/actions/mobskills/mix_final_elixir')
describe('Mix Final Elixir mob skill', function()
    it('restores full HP and MP with RECOVERS_HP_AND_MP', function()
        local skill = require('scripts/actions/mobskills/mix_final_elixir')
        local msg, hp, mp = nil, 0, 0
        local target = {
            getMaxHP = function() return 500 end,
            getMaxMP = function() return 200 end,
            addHP = function(_, v) hp = v end,
            addMP = function(_, v) mp = v end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(hp == 500 and mp == 200 and msg == xi.msg.basic.RECOVERS_HP_AND_MP)
    end)
end)
