describe('Base entity getWeaponHitCount helper', function()
    it('returns PC multihit counts and rejects non-PC receivers', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        local mainHits = player:getWeaponHitCount(false)
        local offHits = player:getWeaponHitCount(true)
        assert(type(mainHits) == 'number', 'getWeaponHitCount(false) should return a number')
        assert(type(offHits) == 'number', 'getWeaponHitCount(true) should return a number')
        -- When a weapon is equipped, hit count is at least 1; empty can be 0.
        assert(mainHits >= 0, 'main hit count should be non-negative')
        assert(offHits >= 0, 'offhand hit count should be non-negative')
        if mainHits > 0 then
            assert(mainHits >= 1 and mainHits <= 8, 'main hit count should be 1..8 when present')
        end

        -- Non-PC receivers return 0.
        assert(mob:getWeaponHitCount(false) == 0, 'mob getWeaponHitCount should be zero')
        assert(npc:getWeaponHitCount(false) == 0, 'NPC getWeaponHitCount should be zero')

        assert(not pcall(player.getWeaponHitCount), 'getWeaponHitCount accepted missing self')
        assert(not pcall(player.getWeaponHitCount, player), 'getWeaponHitCount accepted missing offhand')
        assert(not pcall(player.getWeaponHitCount, player, 'bad'), 'getWeaponHitCount accepted non-bool offhand')
    end)
end)
