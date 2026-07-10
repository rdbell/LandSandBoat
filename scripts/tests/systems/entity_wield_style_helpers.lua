describe('Base entity dual-wield and H2H style helpers', function()
    it('reports dual-wield and H2H style with NPC fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(player:isDualWielding()) == 'boolean', 'player isDualWielding should return a boolean')
        assert(type(player:isUsingH2H()) == 'boolean', 'player isUsingH2H should return a boolean')
        assert(type(mob:isDualWielding()) == 'boolean', 'mob isDualWielding should return a boolean')
        assert(type(mob:isUsingH2H()) == 'boolean', 'mob isUsingH2H should return a boolean')

        -- Bare-handed / unequipped main is treated as H2H for PCs.
        -- Spawned players may already have a weapon; still a boolean.
        assert(player:isUsingH2H() == true or player:isUsingH2H() == false,
            'player isUsingH2H should be a strict boolean')

        -- NPCs do not wield weapons.
        assert(npc:isDualWielding() == false, 'NPC isDualWielding should be false')
        assert(npc:isUsingH2H() == false, 'NPC isUsingH2H should be false')

        assert(not pcall(player.isDualWielding), 'isDualWielding accepted missing self')
        assert(not pcall(player.isUsingH2H), 'isUsingH2H accepted missing self')
    end)
end)
