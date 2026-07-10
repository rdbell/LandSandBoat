describe('Base entity isWeaponTwoHanded helper', function()
    it('reports main-slot two-handed state with NPC fallback', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(player:isWeaponTwoHanded()) == 'boolean',
            'player isWeaponTwoHanded should return a boolean')
        assert(type(mob:isWeaponTwoHanded()) == 'boolean',
            'mob isWeaponTwoHanded should return a boolean')

        -- NPCs always fall back to false.
        assert(npc:isWeaponTwoHanded() == false, 'NPC isWeaponTwoHanded should be false')

        assert(not pcall(player.isWeaponTwoHanded), 'isWeaponTwoHanded accepted missing self')
    end)
end)
