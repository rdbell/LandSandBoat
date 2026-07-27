local eruca = require('scripts/mixins/families/eruca')

local function newMob()
    local mob = { animationSub = 0, casting = nil, engaged = false, listeners = {}, localVars = {}, mobMods = {}, mods = {}, spawnPos = {}, }

    function mob:addListener(event, _, callback) self.listeners[event] = callback end
    function mob:getAnimationSub() return self.animationSub end
    function mob:setAnimationSub(value) self.animationSub = value end
    function mob:setMobMod(mod, value) self.mobMods[mod] = value end
    function mob:setMagicCastingEnabled(value) self.casting = value end
    function mob:setLocalVar(name, value) self.localVars[name] = value end
    function mob:getLocalVar(name) return self.localVars[name] or 0 end
    function mob:isEngaged() return self.engaged end
    function mob:getSpawnPos() return self.spawnPos end
    function mob:checkDistance() return 0 end
    function mob:getMod(mod) return self.mods[mod] or 0 end
    function mob:setMod(mod, value) self.mods[mod] = value end

    return mob
end

describe('Eruca mixin', function()
    it('plans sleep, resleep, wake, and Fire-day Regain transitions', function()
        assert(xi.mix.eruca.resleepTime(100) == 220)
        assert(xi.mix.eruca.roamAction({ subAnimation = 0, currentHour = 18, sleepHour = 18, wakeHour = 6, engaged = false, resleepTime = 0, now = 100 }) == 'sleep')
        assert(xi.mix.eruca.roamAction({ subAnimation = 0, currentHour = 18, sleepHour = 18, wakeHour = 6, engaged = true, resleepTime = 0, now = 100 }) == 'none')
        assert(xi.mix.eruca.roamAction({ subAnimation = 0, currentHour = 18, sleepHour = 18, wakeHour = 6, engaged = false, resleepTime = 50, distanceFromSpawn = 26, now = 100 }) == 'resetResleep')
        assert(xi.mix.eruca.roamAction({ subAnimation = 1, currentHour = 6, sleepHour = 18, wakeHour = 6 }) == 'wake')
        assert(xi.mix.eruca.roamAction({ subAnimation = 1, currentHour = 18, sleepHour = 18, wakeHour = 6 }) == 'none')
        assert(xi.mix.eruca.regainAction(true, 0) == 30)
        assert(xi.mix.eruca.regainAction(true, 30) == nil)
        assert(xi.mix.eruca.regainAction(false, 30) == 0)
        assert(xi.mix.eruca.regainAction(false, 0) == nil)

        local mob = newMob()
        eruca(mob)
        mob.listeners.SPAWN(mob)
        assert(mob:getLocalVar('[eruca]sleepHour') == 18 and mob:getLocalVar('[eruca]wakeHour') == 6)

        mob.animationSub = 1
        mob.listeners.ENGAGE(mob)
        assert(mob.animationSub == 0 and mob.casting == true and mob.mobMods[xi.mobMod.NO_MOVE] == 0)

        mob.listeners.DISENGAGE(mob)
        assert(mob:getLocalVar('ResleepTime') >= GetSystemTime() + 119)
    end)
end)
