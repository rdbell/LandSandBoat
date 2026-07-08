describe('Base entity death type helper bindings', function()
    it('reads and updates mob death type state', function()
        local player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        mob:spawn()

        assert(mob:getDeathType() == 0, 'death type should default to zero')

        mob:setDeathType(2)
        assert(mob:getDeathType() == 2, 'death type did not update')

        mob:setDeathType(260)
        assert(mob:getDeathType() == 4, 'death type should be stored as uint8')

        mob:setDeathType(0)
        assert(mob:getDeathType() == 0, 'death type did not clear')

        assert(not pcall(mob.getDeathType), 'getDeathType accepted missing self')
        assert(not pcall(mob.setDeathType), 'setDeathType accepted missing self')
        assert(not pcall(mob.setDeathType, mob), 'setDeathType accepted missing value')
        assert(not pcall(mob.setDeathType, mob, 'bad'), 'setDeathType accepted non-numeric value')
    end)
end)
