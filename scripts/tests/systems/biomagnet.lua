require('scripts/actions/mobskills/biomagnet')
describe('Biomagnet mob skill', function()
    it('admits closed door and schedules draw-in with DRAWN_IN', function()
        local skill = require('scripts/actions/mobskills/biomagnet')
        local timerDelay, callback, message, setPos = nil, nil, nil, nil
        local mob = {
            getAnimationSub = function() return 0 end,
            timer = function(_, delay, fn)
                timerDelay = delay
                callback = fn
            end,
            getPos = function() return { x = 1, y = 2, z = 3 } end,
            isAlive = function() return true end,
        }
        local target = {
            getHP = function() return 100 end,
            setPos = function(_, x, y, z) setPos = { x, y, z } end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 0)
        assert(timerDelay == 3500 and message == xi.msg.basic.DRAWN_IN)
        callback(mob)
        assert(setPos[1] == 1 and setPos[2] == 2 and setPos[3] == 3)
    end)
end)
