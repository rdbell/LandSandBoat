local gargouille = require('scripts/mixins/families/gargouille')

local function newMob()
    local mob = { animationSub = 0, listeners = {}, localVars = {}, mobMods = {}, mods = {} }
    function mob:addListener(event, _, callback) self.listeners[event] = callback end
    function mob:getAnimationSub() return self.animationSub end
    function mob:setAnimationSub(value) self.animationSub = value end
    function mob:setMobMod(mod, value) self.mobMods[mod] = value end
    function mob:addMod(mod, value) self.mods[mod] = (self.mods[mod] or 0) + value end
    function mob:delMod(mod, value) self.mods[mod] = (self.mods[mod] or 0) - value end
    function mob:setLocalVar(name, value) self.localVars[name] = value end
    function mob:getLocalVar(name) return self.localVars[name] or 0 end
    return mob
end

describe('Gargouille mixin', function()
    it('alternates stance when its form timer expires', function()
        assert(xi.mix.gargouille.formTimer(100, 180) == 280)
        assert(xi.mix.gargouille.shouldChangeStance(279, 280) == false)
        assert(xi.mix.gargouille.shouldChangeStance(280, 280) == true)

        local mob = newMob()
        gargouille(mob)
        mob.listeners.SPAWN(mob)
        assert(mob.animationSub == 4 and mob.mobMods[xi.mobMod.SKILL_LIST] == 118)

        mob:setLocalVar('formTimer', 0)
        mob.listeners.ROAM_TICK(mob)
        assert(mob.animationSub == 5 and mob.mobMods[xi.mobMod.SKILL_LIST] == 117)
        assert(mob.mods[xi.mod.EVA] == 60 and mob.mods[xi.mod.DMGMAGIC] == -1250)

        mob:setLocalVar('formTimer', 0)
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.animationSub == 4 and mob.mobMods[xi.mobMod.SKILL_LIST] == 118)
        assert(mob.mods[xi.mod.EVA] == 0 and mob.mods[xi.mod.DMGMAGIC] == 0)
    end)
end)
