require('scripts/globals/player')

describe('Player mount Mimeo Jewel condition', function()
    it('breaks the jewel only when mounting in Attohwa Chasm', function()
        local removed = 0
        local messages = 0
        local hasJewel = true
        local player = {
            getZoneID = function() return xi.zone.ATTOHWA_CHASM end,
            hasKeyItem = function(_, keyItem) return hasJewel and keyItem == xi.ki.MIMEO_JEWEL end,
            messageSpecial = function(_, _, keyItem)
                assert(keyItem == xi.ki.MIMEO_JEWEL)
                messages = messages + 1
            end,
            delKeyItem = function(_, keyItem)
                assert(keyItem == xi.ki.MIMEO_JEWEL)
                removed = removed + 1
            end,
        }

        xi.player.onPlayerMount(player)
        assert(removed == 1)
        assert(messages == 1)

        hasJewel = false
        xi.player.onPlayerMount(player)
        assert(removed == 1)
        assert(messages == 1)

        player.getZoneID = function() return xi.zone.BASTOK_MARKETS end
        xi.player.onPlayerMount(player)
        assert(removed == 1)
        assert(messages == 1)
    end)
end)
