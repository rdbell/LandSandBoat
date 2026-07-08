describe('Base entity Treasure Hunter level helper bindings', function()
    it('reads and updates mob Treasure Hunter levels while invalid receivers fall back', function()
        local player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:setTHlevel(0)
        assert(mob:getTHlevel() == 0, 'Treasure Hunter level did not clear')

        mob:setTHlevel(5)
        assert(mob:getTHlevel() == 5, 'Treasure Hunter level did not update')

        mob:setTHlevel(-2)
        assert(mob:getTHlevel() == -2, 'Treasure Hunter level did not preserve signed values')

        mob:setTHlevel(0)
        assert(mob:getTHlevel() == 0, 'Treasure Hunter level did not clear after update')

        assert(npc:getTHlevel() == 0, 'NPC getTHlevel should fall back to zero')
        assert(pcall(npc.setTHlevel, npc, 9), 'NPC setTHlevel should accept a valid no-op')
        assert(npc:getTHlevel() == 0, 'NPC setTHlevel should stay at zero')
        assert(player:getTHlevel() == 0, 'player getTHlevel should fall back to zero')
        assert(pcall(player.setTHlevel, player, 10), 'player setTHlevel should accept a valid no-op')
        assert(player:getTHlevel() == 0, 'player setTHlevel should stay at zero')

        assert(not pcall(mob.getTHlevel), 'getTHlevel accepted missing self')
        assert(not pcall(mob.setTHlevel), 'setTHlevel accepted missing self')
        assert(not pcall(mob.setTHlevel, mob), 'setTHlevel accepted missing value')
        assert(not pcall(mob.setTHlevel, mob, 'bad'), 'setTHlevel accepted non-numeric value')
        assert(not pcall(player.setTHlevel, player, 'bad'), 'invalid setTHlevel receiver accepted non-numeric value')
    end)
end)
