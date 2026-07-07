describe('Base entity status helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates spawned player status', function()
        local initialStatus = player:getStatus()

        assert(type(initialStatus) == 'number', 'Player status was not numeric')
        assert(initialStatus == xi.status.NORMAL, 'Player initial status was not NORMAL')

        player:setStatus(xi.status.INVISIBLE)
        assert(player:getStatus() == xi.status.INVISIBLE, 'Player status did not become INVISIBLE')

        player:setStatus(xi.status.CUTSCENE_ONLY)
        assert(player:getStatus() == xi.status.CUTSCENE_ONLY, 'Player status did not become CUTSCENE_ONLY')

        player:setStatus(initialStatus)
        assert(player:getStatus() == initialStatus, 'Player status did not restore')
    end)
end)
