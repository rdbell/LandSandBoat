describe('Chocobo renter finish', function()
    it('consumes an accepted Chocopass and mounts for its stored duration', function()
        local writes, mounted, tradeCompleted, petDespawned = {}, 0, false, false
        local player = {
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]DestCity')
                return 0
            end,
            getMainLvl = function() return 1 end,
            getZoneID = function() return xi.zone.WINDURST_WOODS end,
            getLocalVar = function(_, key)
                if key == 'Chocopass' then return 1 end
                if key == 'ChocopassDuration' then return 180 end
            end,
            setLocalVar = function(_, key, value) writes[key] = value end,
            tradeComplete = function() tradeCompleted = true end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.MOUNTED and params.duration == 180 and params.silent)
                mounted = mounted + 1
            end,
            despawnPet = function() petDespawned = true end,
            setPos = function() end,
        }

        xi.chocobo.renterOnEventFinish(player, 100, 0, 100)
        assert(tradeCompleted and writes.Chocopass == 0 and writes.ChocopassDuration == 0)
        assert(mounted == 1 and petDespawned)
    end)
end)
