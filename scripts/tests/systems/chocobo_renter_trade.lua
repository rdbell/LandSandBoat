describe('Chocobo renter trade', function()
    local function player(zone, level, hasLicense, ridingTime)
        local writes = {}
        local event
        return {
            getZoneID = function() return zone end,
            getMainLvl = function() return level end,
            hasKeyItem = function(_, keyItem)
                assert(keyItem == xi.ki.CHOCOBO_LICENSE)
                return hasLicense
            end,
            getMod = function() return ridingTime end,
            getGil = function() return 999 end,
            setLocalVar = function(_, key, value) writes[key] = value end,
            startEvent = function(_, ...) event = { ... } end,
            writes = function() return writes end,
            event = function() return event end,
        }
    end

    it('admits Chocopasses and eligible tickets with their distinct durations', function()
        local originalTradeHasExactly = npcUtil.tradeHasExactly
        local tradeItem
        npcUtil.tradeHasExactly = function(_, item) return item == tradeItem end

        local ok, err = pcall(function()
            tradeItem = xi.item.FREE_CHOCOPASS
            local freePass = player(xi.zone.WINDURST_WOODS, 1, false, 0)
            xi.chocobo.renterOnTrade(freePass, {}, {}, 100, 200)
            assert(freePass.writes().Chocopass == 1 and freePass.writes().ChocopassDuration == 180)
            assert(freePass.event()[1] == 100 and freePass.event()[2] == 0 and freePass.event()[3] == 999)

            tradeItem = xi.item.CHOCOBO_TICKET
            local ticket = player(xi.zone.LA_THEINE_PLATEAU, 20, true, 2)
            xi.chocobo.renterOnTrade(ticket, {}, {}, 100, 200)
            assert(ticket.writes().Chocopass == 1 and ticket.writes().ChocopassDuration == 1920)
            assert(ticket.event()[1] == 100 and ticket.event()[2] == 0 and ticket.event()[3] == 999)

            local pastZone = player(xi.zone.SOUTHERN_SAN_DORIA_S, 99, true, 0)
            xi.chocobo.renterOnTrade(pastZone, {}, {}, 100, 200)
            assert(pastZone.event() == nil and next(pastZone.writes()) == nil)

            local unlicensed = player(xi.zone.LA_THEINE_PLATEAU, 20, false, 0)
            xi.chocobo.renterOnTrade(unlicensed, {}, {}, 100, 200)
            assert(unlicensed.event() and unlicensed.event()[1] == 200)

            local underlevel = player(xi.zone.LA_THEINE_PLATEAU, 19, true, 0)
            xi.chocobo.renterOnTrade(underlevel, {}, {}, 100, 200)
            assert(underlevel.event() and underlevel.event()[1] == 200)
        end)

        npcUtil.tradeHasExactly = originalTradeHasExactly
        assert(ok, err)
    end)
end)
