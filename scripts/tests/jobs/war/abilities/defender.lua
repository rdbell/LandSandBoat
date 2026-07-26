describe('Defender', function()
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                job   = xi.job.WAR,
                level = 49, -- Defender is 25% DEFP until 50
            })
    end)

    it('applies its self-origin defense stance', function()
        player.actions:useAbility(player, xi.jobAbility.DEFENDER)
        xi.test.world:tick()

        player.assert
            :hasEffect(xi.effect.DEFENDER)
            :hasModifier(xi.mod.DEFP, 25)
            :hasModifier(xi.mod.ATTP, -25)
    end)
end)
