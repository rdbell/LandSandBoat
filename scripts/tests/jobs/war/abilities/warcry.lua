describe('Warcry', function()
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                job   = xi.job.WAR,
                level = 75,
            })
    end)

    it('truncates the computed attack percentage into status-effect power', function()
        player.actions:useAbility(player, xi.jobAbility.WARCRY)
        xi.test.world:tick()

        -- (floor(75 / 4 + 4.75) / 256) * 100 = 8.984375, stored as uint16 8.
        player.assert
            :hasEffect(xi.effect.WARCRY)
            :hasModifier(xi.mod.ATTP, 8)
            :hasModifier(xi.mod.RATTP, 8)
    end)
end)
