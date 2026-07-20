require('scripts/actions/mobskills/bugle_call')
describe('Bugle Call mob skill', function()
    it('admits level 63+ and schedules TP plus help spawn', function()
        local skill = require('scripts/actions/mobskills/bugle_call')
        local timers, msg, addedTP = {}, nil, nil
        local player = {}
        local mob = {
            getMainLvl = function() return 62 end,
            getID = function() return 200 end,
            getTarget = function() return player end,
            timer = function(_, ms, fn) timers[#timers + 1] = { ms, fn } end,
            addTP = function(_, v) addedTP = v end,
            messageText = function() end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getMainLvl = function() return 63 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
        assert(#timers == 2 and timers[1][1] == 4000 and timers[2][1] == 3000)
        timers[1][2](mob)
        assert(addedTP == 1000)
    end)
end)
