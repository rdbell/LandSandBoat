describe('Base entity gear set mod helpers', function()
    it('applies and clears gear set mods for PCs only', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')

        local modId = xi.mod.ATT
        local before = player:getMod(modId)

        player:addGearSetMod(1, modId, 7)
        assert(player:getMod(modId) == before + 7, 'addGearSetMod should apply modifier amount')

        player:addGearSetMod(2, modId, 3)
        assert(player:getMod(modId) == before + 10, 'second addGearSetMod should stack')

        player:clearGearSetMods()
        assert(player:getMod(modId) == before, 'clearGearSetMods should remove gear set amounts')

        -- Negative amounts (damage-taken gear sets) apply as signed int16 via uint16 binding.
        local dmgMod = xi.mod.DMGMAGIC
        local dmgBefore = player:getMod(dmgMod)
        player:addGearSetMod(3, dmgMod, -500)
        assert(player:getMod(dmgMod) == dmgBefore - 500, 'negative addGearSetMod should reduce mod')
        player:clearGearSetMods()
        assert(player:getMod(dmgMod) == dmgBefore, 'clear should undo negative gear set amount')

        -- Non-PC: helpers are no-ops (may warn).
        local mobBefore = mob:getMod(modId)
        mob:addGearSetMod(1, modId, 9)
        assert(mob:getMod(modId) == mobBefore, 'mob addGearSetMod should not change mod')
        mob:clearGearSetMods()

        assert(not pcall(player.addGearSetMod, player), 'addGearSetMod accepted missing args')
        -- clearGearSetMods requires self only.
        assert(not pcall(player.clearGearSetMods), 'clearGearSetMods accepted missing self')
    end)
end)
