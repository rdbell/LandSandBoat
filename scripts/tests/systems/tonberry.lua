local tonberry = require('scripts/mixins/families/tonberry')

describe('Tonberry mixin', function()
    it('increments in-zone alliance grudge kills without exceeding the cap', function()
        assert(xi.mix.tonberry.nextGrudgeKills(479, 5, 5) == 480)
        assert(xi.mix.tonberry.nextGrudgeKills(480, 5, 5) == nil)
        assert(xi.mix.tonberry.nextGrudgeKills(20, 6, 5) == nil)

        local function member(zone, kills)
            return {
                zone = zone,
                kills = kills,
                getZoneID = function(self) return self.zone end,
                getCharVar = function(self) return self.kills end,
                setCharVar = function(self, _, value) self.kills = value end,
            }
        end
        local player = member(5, 479)
        local capped = member(5, 480)
        local elsewhere = member(6, 20)
        function player:getAlliance() return { self, capped, elsewhere } end

        local mob = { listeners = {} }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        tonberry(mob)
        assert(mob.listeners.DEATH)

        mob.listeners.DEATH(mob, player)
        assert(player.kills == 480 and capped.kills == 480 and elsewhere.kills == 20)
        mob.listeners.DEATH(mob, nil)
        assert(player.kills == 480 and capped.kills == 480 and elsewhere.kills == 20)
    end)
end)
