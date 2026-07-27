describe('Chocobo renter race finish', function()
    it('starts an accepted staged race and mounts without charging a rental', function()
        local writes, mounted, petDespawned, movedTo, raceOption, gilCalled, salesCalled = {}, 0, false, nil, nil, false, false
        local oldBeginRace = xi.chocoboGame.beginRace
        local oldIncreaseSales = xi.chocobo.increaseSales
        xi.chocoboGame.beginRace = function(_, option) raceOption = option end
        xi.chocobo.increaseSales = function() salesCalled = true end

        local player = {
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]DestCity')
                return xi.zone.SAUROMUGUE_CHAMPAIGN
            end,
            getMainLvl = function() return 20 end,
            getZoneID = function() return xi.zone.WINDURST_WOODS end,
            getLocalVar = function(_, key)
                if key == 'Chocopass' then return 0 end
                if key == '[CHOCOBO]price' then return 70 end
            end,
            setLocalVar = function(_, key, value) writes[key] = value end,
            getMod = function(_, mod)
                assert(mod == xi.mod.CHOCOBO_RIDING_TIME)
                return 2
            end,
            delGil = function() gilCalled = true end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.MOUNTED and params.duration == 1920 and params.silent)
                mounted = mounted + 1
            end,
            despawnPet = function() petDespawned = true end,
            setPos = function(_, x, y, z, rot, zone)
                movedTo = { x, y, z, rot, zone }
            end,
        }

        xi.chocobo.renterOnEventFinish(player, 100, 0, 100)
        xi.chocoboGame.beginRace = oldBeginRace
        xi.chocobo.increaseSales = oldIncreaseSales

        assert(raceOption == 0 and writes['[CHOCOBO]price'] == 0)
        assert(mounted == 1 and petDespawned and not gilCalled and not salesCalled)
        assert(movedTo[1] == -122 and movedTo[2] == -4 and movedTo[3] == -520 and movedTo[4] == 0 and movedTo[5] == xi.zone.EAST_SARUTABARUTA)
    end)

    it('despawns the pet and declines a staged race without mounting', function()
        local mounted, petDespawned, raceOption, quoteCleared = 0, false, nil, false
        local oldBeginRace = xi.chocoboGame.beginRace
        xi.chocoboGame.beginRace = function(_, option) raceOption = option end

        local player = {
            getCharVar = function() return xi.zone.SAUROMUGUE_CHAMPAIGN end,
            setLocalVar = function(_, key)
                if key == '[CHOCOBO]price' then quoteCleared = true end
            end,
            despawnPet = function() petDespawned = true end,
            addStatusEffect = function() mounted = mounted + 1 end,
        }

        xi.chocobo.renterOnEventFinish(player, 100, 1, 100)
        xi.chocoboGame.beginRace = oldBeginRace

        assert(raceOption == 1 and petDespawned and mounted == 0 and not quoteCleared)
    end)
end)
