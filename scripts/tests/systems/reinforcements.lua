require('scripts/actions/mobskills/reinforcements')
describe('Reinforcements mob skill', function()
    it('admits open rampart with unspawned adds and schedules spawn', function()
        local skill = require('scripts/actions/mobskills/reinforcements')
        local msg, delay = nil, nil
        local vars = { spawnCount = 1, spawnOffset = 0 }
        local instance = {}
        local fakeMob = { isSpawned = function() return false end }
        local originalGet = _G.GetMobByID
        local originalShuffle = utils.shuffle
        _G.GetMobByID = function() return fakeMob end
        utils.shuffle = function(t) return t end
        local mob = {
            getLocalVar = function(_, k) return vars[k] or 0 end,
            setLocalVar = function(_, k, v) vars[k] = v end,
            getAnimationSub = function() return 1 end,
            getInstance = function() return instance end,
            getID = function() return 100 end,
            timer = function(_, d) delay = d end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(delay == 2500 and msg == xi.msg.basic.NONE)
        -- no instance => skill check fails
        mob.getInstance = function() return nil end
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        _G.GetMobByID = originalGet
        utils.shuffle = originalShuffle
    end)
end)
