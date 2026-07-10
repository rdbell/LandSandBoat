describe('Base entity learned weaponskill helpers', function()
    it('adds, queries, and removes PC weaponskill unlocks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(player:hasLearnedWeaponskill(xi.wsUnlock.DECIMATION) == false,
            'decimation should start unlearned')

        player:addLearnedWeaponskill(xi.wsUnlock.DECIMATION)
        assert(player:hasLearnedWeaponskill(xi.wsUnlock.DECIMATION) == true,
            'addLearnedWeaponskill should set decimation')
        assert(player:hasLearnedWeaponskill(xi.wsUnlock.ASURAN_FISTS) == false,
            'unrelated unlock should remain clear')

        player:addLearnedWeaponskill(xi.wsUnlock.ASURAN_FISTS)
        player:delLearnedWeaponskill(xi.wsUnlock.DECIMATION)
        assert(player:hasLearnedWeaponskill(xi.wsUnlock.DECIMATION) == false,
            'delLearnedWeaponskill should clear decimation')
        assert(player:hasLearnedWeaponskill(xi.wsUnlock.ASURAN_FISTS) == true,
            'delLearnedWeaponskill should not clear other unlocks')

        player:addLearnedWeaponskill(0)
        player:addLearnedWeaponskill(63)
        player:addLearnedWeaponskill(64)
        assert(player:hasLearnedWeaponskill(0) == true, 'unlock id 0 should be valid')
        assert(player:hasLearnedWeaponskill(63) == true, 'unlock id 63 should be valid')
        assert(player:hasLearnedWeaponskill(64) == false, 'unlock id 64 should be rejected')

        mob:addLearnedWeaponskill(xi.wsUnlock.DECIMATION)
        assert(mob:hasLearnedWeaponskill(xi.wsUnlock.DECIMATION) == false,
            'mob add/has should fall back to false')
        npc:addLearnedWeaponskill(xi.wsUnlock.DECIMATION)
        assert(npc:hasLearnedWeaponskill(xi.wsUnlock.DECIMATION) == false,
            'NPC add/has should fall back to false')

        assert(not pcall(player.hasLearnedWeaponskill), 'hasLearnedWeaponskill accepted missing self')
        assert(not pcall(player.hasLearnedWeaponskill, player), 'hasLearnedWeaponskill accepted missing unlock id')
        assert(not pcall(player.hasLearnedWeaponskill, player, 'bad'), 'hasLearnedWeaponskill accepted non-numeric unlock id')
        assert(not pcall(player.addLearnedWeaponskill), 'addLearnedWeaponskill accepted missing self')
        assert(not pcall(player.addLearnedWeaponskill, player), 'addLearnedWeaponskill accepted missing unlock id')
        assert(not pcall(player.addLearnedWeaponskill, player, 'bad'), 'addLearnedWeaponskill accepted non-numeric unlock id')
        assert(not pcall(player.delLearnedWeaponskill), 'delLearnedWeaponskill accepted missing self')
        assert(not pcall(player.delLearnedWeaponskill, player), 'delLearnedWeaponskill accepted missing unlock id')
        assert(not pcall(player.delLearnedWeaponskill, player, 'bad'), 'delLearnedWeaponskill accepted non-numeric unlock id')
    end)
end)
