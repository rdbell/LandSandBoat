require('scripts/actions/mobskills/starlight')
describe('Starlight mob skill', function()
    it('restores MP from club skill and TP', function()
        local skill = require('scripts/actions/mobskills/starlight')
        local msg, mp = nil, 0
        local mob = {
            getMainLvl = function() return 99 end,
            getMainJob = function() return 1 end,
            getMaxSkillLevel = function() return 99 end,
            addMP = function(_, a) mp = a end,
        }
        local sk = { getTP = function() return 1000 end, setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 9)
        assert(mp == 9 and msg == xi.msg.basic.SKILL_RECOVERS_MP)
    end)
end)
