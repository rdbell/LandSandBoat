describe('Base entity pet ability helper binding', function()
    it('accepts ability IDs as the current no-op helper', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        assert(pcall(player.petAbility, player, xi.jobAbility.POISON_NAILS), 'petAbility rejected a valid ability ID')
        assert(not pcall(player.petAbility), 'petAbility accepted missing self')
        assert(not pcall(player.petAbility, player), 'petAbility accepted missing ability ID')
        assert(not pcall(player.petAbility, player, 'bad'), 'petAbility accepted nonnumeric ability ID')
    end)
end)
