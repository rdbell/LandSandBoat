describe('Chocobo renter regular finish', function()
    it('charges gil, increases sales, and mounts a level-20 renter', function()
        local writes, mounted, petDespawned, movedTo, salesZone = {}, 0, false, nil, nil
        local oldIncreaseSales = xi.chocobo.increaseSales
        xi.chocobo.increaseSales = function(zoneId) salesZone = zoneId end

        local player = {
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]DestCity')
                return 0
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
                return 3
            end,
            delGil = function(_, amount)
                assert(amount == 70)
                return true
            end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.MOUNTED and params.duration == 1980 and params.silent)
                mounted = mounted + 1
            end,
            despawnPet = function() petDespawned = true end,
            setPos = function(_, x, y, z, rot, zone)
                movedTo = { x, y, z, rot, zone }
            end,
        }

        xi.chocobo.renterOnEventFinish(player, 100, 0, 100)
        xi.chocobo.increaseSales = oldIncreaseSales

        assert(writes['[CHOCOBO]price'] == 0 and salesZone == xi.zone.WINDURST_WOODS)
        assert(mounted == 1 and petDespawned)
        assert(movedTo[1] == -122 and movedTo[2] == -4 and movedTo[3] == -520 and movedTo[4] == 0 and movedTo[5] == xi.zone.EAST_SARUTABARUTA)
    end)

    it('charges allied notes for a past renter at the default duration', function()
        local writes, mounted, notesDebited, salesZone = {}, 0, nil, nil
        local oldIncreaseSales = xi.chocobo.increaseSales
        xi.chocobo.increaseSales = function(zoneId) salesZone = zoneId end

        local player = {
            getCharVar = function() return 0 end,
            getMainLvl = function() return 15 end,
            getZoneID = function() return xi.zone.SOUTHERN_SAN_DORIA_S end,
            getLocalVar = function(_, key)
                if key == 'Chocopass' then return 0 end
                if key == '[CHOCOBO]price' then return 60 end
            end,
            setLocalVar = function(_, key, value) writes[key] = value end,
            getCurrency = function(_, currency)
                assert(currency == 'allied_notes')
                return 60
            end,
            delCurrency = function(_, currency, amount)
                assert(currency == 'allied_notes' and amount == 60)
                notesDebited = amount
            end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.MOUNTED and params.duration == 900 and params.silent)
                mounted = mounted + 1
            end,
            despawnPet = function() end,
            setPos = function() end,
        }

        xi.chocobo.renterOnEventFinish(player, 100, 0, 100)
        xi.chocobo.increaseSales = oldIncreaseSales

        assert(writes['[CHOCOBO]price'] == 0 and notesDebited == 60)
        assert(mounted == 1 and salesZone == xi.zone.SOUTHERN_SAN_DORIA_S)
    end)

    it('clears the quote but does not mount when gil debit fails', function()
        local writes, mounted, petDespawned, moved = {}, 0, false, false
        local player = {
            getCharVar = function() return 0 end,
            getMainLvl = function() return 20 end,
            getZoneID = function() return xi.zone.WINDURST_WOODS end,
            getLocalVar = function(_, key)
                if key == 'Chocopass' then return 0 end
                if key == '[CHOCOBO]price' then return 70 end
            end,
            setLocalVar = function(_, key, value) writes[key] = value end,
            getMod = function() return 0 end,
            delGil = function() return false end,
            getName = function() return 'test player' end,
            addStatusEffect = function() mounted = mounted + 1 end,
            despawnPet = function() petDespawned = true end,
            setPos = function() moved = true end,
        }

        xi.chocobo.renterOnEventFinish(player, 100, 0, 100)

        assert(writes['[CHOCOBO]price'] == 0)
        assert(mounted == 0 and not petDespawned and not moved)
    end)
end)
