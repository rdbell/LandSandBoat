describe('Chocobo renter sales state', function()
    it('resets sales when a renter zone initializes', function()
        local info = xi.chocobo.chocoboInfo[xi.zone.LA_THEINE_PLATEAU]
        info.sales = 12

        xi.chocobo.initZone({
            getID = function() return xi.zone.LA_THEINE_PLATEAU end,
            getName = function() return 'La Theine Plateau' end,
        })

        assert(info.sales == 0)
    end)
end)
