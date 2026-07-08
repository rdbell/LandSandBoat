describe('Base entity zone identity helper bindings', function()
    it('reads zone identity and region metadata from spawned players', function()
        local player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })

        assert(player:getZoneID() == xi.zone.WEST_RONFAURE, 'getZoneID did not return the current zone')
        assert(player:getZoneName() == 'West_Ronfaure', 'getZoneName did not return the zone name')
        assert(player:getPreviousZone() == 0, 'newly spawned player previous zone was not zero')
        assert(player:getPreviousZoneLineID() == 0, 'newly spawned player previous zoneline was not zero')
        assert(player:getCurrentRegion() == xi.region.RONFAURE, 'getCurrentRegion did not return Ronfaure')
        assert(player:getContinentID() == xi.continent.THE_MIDDLE_LANDS, 'getContinentID did not return the Middle Lands')
    end)

end)
