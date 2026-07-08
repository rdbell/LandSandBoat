describe('Gambit', function()
    ---@type CClientEntityPair
    local player
    ---@type CTestEntity
    local euvhi

    before_each(function()
        player = xi.test.world:spawnPlayer(
        {
            job = xi.job.RUN,
            level = 75,
            zone = xi.zone.ALTAIEU
        })

        -- Find a mob to reduce res
        euvhi = player.entities:moveTo('Aweuvhi')
    end)

    it('reduces wind res', function()
        local origWind = euvhi:getMod(xi.mod.WIND_SDT)
        player.actions:useAbility(player, xi.jobAbility.FLABRA)
        xi.test.world:tick()
        player:resetRecasts()
        player.actions:useAbility(player, xi.jobAbility.FLABRA)
        xi.test.world:tick()
        player:resetRecasts()
        player.actions:useAbility(player, xi.jobAbility.FLABRA)
        xi.test.world:tick()

        assert(
            player:getActiveRuneCount() == 3,
            'Using Flabra 3 times should have 3 runes'
        )

        player.actions:useAbility(euvhi, xi.jobAbility.GAMBIT)
        xi.test.world:tick()

        euvhi.assert
            :hasEffect(xi.effect.GAMBIT)
            :hasModifier(xi.mod.WIND_SDT, origWind + 3000)

        euvhi:delStatusEffect(xi.effect.GAMBIT)
        xi.test.world:tick()

        assert(
            euvhi:getMod(xi.mod.WIND_SDT) == origWind,
            'Losing rayke should revert WIND_SDT'
        )
    end)

    it('lists and removes active runes', function()
        player:addStatusEffect(xi.effect.PROTECT, { power = 1, duration = 60, origin = player })
        player:addStatusEffect(xi.effect.SULPOR, { power = 1, duration = 60, origin = player })
        xi.test.world:skipTime(1)
        player:addStatusEffect(xi.effect.SULPOR, { power = 1, duration = 60, origin = player })
        xi.test.world:skipTime(1)
        player:addStatusEffect(xi.effect.IGNIS, { power = 1, duration = 60, origin = player })
        xi.test.world:skipTime(1)
        player:addStatusEffect(xi.effect.LUX, { power = 1, duration = 60, origin = player })

        local runes = player:getAllRuneEffects()
        assert(runes[1] == xi.effect.IGNIS, 'expected first rune to be Ignis')
        assert(runes[2] == xi.effect.SULPOR, 'expected second rune to be Sulpor')
        assert(runes[3] == xi.effect.SULPOR, 'expected third rune to be Sulpor')
        assert(runes[4] == xi.effect.LUX, 'expected fourth rune to be Lux')
        assert(player:getActiveRuneCount() == 4, 'expected four active runes')
        assert(player:getHighestRuneEffect() == xi.effect.SULPOR, 'expected duplicated Sulpor to be highest')
        assert(player:getNewestRuneEffect() == xi.effect.LUX, 'expected Lux to be newest')

        player:removeNewestRune()
        assert(player:getActiveRuneCount() == 3, 'expected newest rune to be removed')
        assert(not player:hasStatusEffect(xi.effect.LUX), 'newest rune should be removed')

        player:removeOldestRune()
        assert(player:getActiveRuneCount() == 2, 'expected oldest rune to be removed')
        assert(player:hasStatusEffect(xi.effect.IGNIS), 'newer rune should remain')
        assert(player:hasStatusEffect(xi.effect.SULPOR), 'duplicate rune should still have one copy remaining')

        player:removeAllRunes()
        assert(player:getActiveRuneCount() == 0, 'expected all runes to be removed')
        assert(player:hasStatusEffect(xi.effect.PROTECT), 'non-rune effect should remain')

        assert(not pcall(player.getAllRuneEffects), 'getAllRuneEffects accepted missing self')
        assert(not pcall(player.getActiveRuneCount), 'getActiveRuneCount accepted missing self')
        assert(not pcall(player.getHighestRuneEffect), 'getHighestRuneEffect accepted missing self')
        assert(not pcall(player.getNewestRuneEffect), 'getNewestRuneEffect accepted missing self')
        assert(not pcall(player.removeNewestRune), 'removeNewestRune accepted missing self')
        assert(not pcall(player.removeOldestRune), 'removeOldestRune accepted missing self')
        assert(not pcall(player.removeAllRunes), 'removeAllRunes accepted missing self')
    end)
end)
