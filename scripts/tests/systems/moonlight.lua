require('scripts/actions/mobskills/moonlight')
describe('Moonlight mob skill', function()
    it('restores party MP scaled by club skill and TP', function()
        local skill = require('scripts/actions/mobskills/moonlight')
        local msg, added = nil, {}
        local member = { isAlive = function() return true end, addMP = function(_, a) added[#added+1] = a end }
        local mob = {
            getMainLvl = function() return 99 end,
            getMainJob = function() return 1 end,
            getMaxSkillLevel = function() return 99 end,
            getParty = function() return { member } end,
            checkDistance = function() return 1 end,
        }
        local sk = { getTP = function() return 1000 end, setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        local ret = skill.onMobWeaponSkill(mob, {}, sk, {})
        assert(msg == xi.msg.basic.SKILL_RECOVERS_MP and added[1] == ret and ret == 10)
    end)
end)
