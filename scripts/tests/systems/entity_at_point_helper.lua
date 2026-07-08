describe('Base entity atPoint helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('checks numeric and sequence table points with LSB tolerance', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')

        mob:setPos(1, 2, 3, 32)

        assert(mob:atPoint(1, 2, 3), 'atPoint numeric exact point mismatch')
        assert(mob:atPoint({ 1, 2, 3 }), 'atPoint sequence table exact point mismatch')
        assert(mob:atPoint(1.01, 2, 3), 'atPoint did not include 0.01 threshold')
        assert(not mob:atPoint(1.011, 2, 3), 'atPoint accepted point outside threshold')
        assert(not mob:atPoint(1, 2.011, 3), 'atPoint ignored vertical distance')
        assert(not mob:atPoint(), 'atPoint missing point unexpectedly matched origin')

        assert(not pcall(mob.atPoint), 'atPoint accepted missing self')
        assert(not mob:atPoint({ 1, 2 }), 'atPoint incomplete table unexpectedly matched point')
    end)
end)
