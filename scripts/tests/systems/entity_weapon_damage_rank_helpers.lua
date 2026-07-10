describe('Base entity weapon damage rank helpers', function()
    it('reads main/offhand/ranged weapon ranks with NPC fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(player:getWeaponDmgRank()) == 'number', 'getWeaponDmgRank should return a number')
        assert(type(player:getOffhandDmgRank()) == 'number', 'getOffhandDmgRank should return a number')
        assert(type(player:getRangedDmgRank()) == 'number', 'getRangedDmgRank should return a number')

        -- Rank is floor(weapon damage adjustments / 9); always a non-negative integer.
        assert(player:getWeaponDmgRank() >= 0, 'getWeaponDmgRank should be non-negative')
        assert(player:getOffhandDmgRank() >= 0, 'getOffhandDmgRank should be non-negative')
        assert(player:getRangedDmgRank() >= 0, 'getRangedDmgRank should be non-negative')

        local mobRank = mob:getWeaponDmgRank()
        assert(type(mobRank) == 'number', 'mob getWeaponDmgRank should return a number')
        assert(mobRank >= 0, 'mob getWeaponDmgRank should be non-negative')

        assert(npc:getWeaponDmgRank() == 0, 'NPC getWeaponDmgRank should be zero')
        assert(npc:getOffhandDmgRank() == 0, 'NPC getOffhandDmgRank should be zero')
        assert(npc:getRangedDmgRank() == 0, 'NPC getRangedDmgRank should be zero')

        assert(not pcall(player.getWeaponDmgRank), 'getWeaponDmgRank accepted missing self')
        assert(not pcall(player.getOffhandDmgRank), 'getOffhandDmgRank accepted missing self')
        assert(not pcall(player.getRangedDmgRank), 'getRangedDmgRank accepted missing self')
    end)
end)
