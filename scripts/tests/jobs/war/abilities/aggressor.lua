describe('Aggressor', function()
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                job   = xi.job.WAR,
                level = 45,
            })
    end)

    it('applies its self-origin attack-accuracy stance', function()
        player.actions:useAbility(player, xi.jobAbility.AGGRESSOR)
        xi.test.world:tick()

        player.assert
            :hasEffect(xi.effect.AGGRESSOR)
            :hasModifier(xi.mod.ACC, 25)
            :hasModifier(xi.mod.EVA, -25)
    end)
end)
