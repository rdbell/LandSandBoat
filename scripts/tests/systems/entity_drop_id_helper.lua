describe('Base entity drop ID helper bindings', function()
    it('reads and updates mob drop IDs while invalid receivers fall back', function()
        local player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')
        mob:spawn()

        mob:setDropID(0)
        assert(mob:getDropID() == 0, 'drop ID did not clear')

        mob:setDropID(2408)
        assert(mob:getDropID() == 2408, 'drop ID did not update')

        mob:setDropID(0)
        assert(mob:getDropID() == 0, 'drop ID did not clear after update')

        assert(npc:getDropID() == 0, 'NPC getDropID should fall back to zero')
        assert(pcall(npc.setDropID, npc, 999), 'NPC setDropID should accept a valid no-op')
        assert(npc:getDropID() == 0, 'NPC setDropID should stay at zero')
        assert(player:getDropID() == 0, 'player getDropID should fall back to zero')
        assert(pcall(player.setDropID, player, 999), 'player setDropID should accept a valid no-op')
        assert(player:getDropID() == 0, 'player setDropID should stay at zero')

        assert(not pcall(mob.getDropID), 'getDropID accepted missing self')
        assert(not pcall(mob.setDropID), 'setDropID accepted missing self')
        assert(not pcall(mob.setDropID, mob), 'setDropID accepted missing value')
        assert(not pcall(mob.setDropID, mob, 'bad'), 'setDropID accepted non-numeric value')
        assert(not pcall(player.setDropID, player, 'bad'), 'invalid setDropID receiver accepted non-numeric value')
    end)
end)
