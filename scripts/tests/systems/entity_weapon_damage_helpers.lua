describe('Base entity weapon damage helpers', function()
    it('reads main/offhand/ranged/ammo damage with NPC fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        -- All four helpers return numbers for battle entities.
        assert(type(player:getWeaponDmg()) == 'number', 'getWeaponDmg should return a number')
        assert(type(player:getOffhandDmg()) == 'number', 'getOffhandDmg should return a number')
        assert(type(player:getRangedDmg()) == 'number', 'getRangedDmg should return a number')
        assert(type(player:getAmmoDmg()) == 'number', 'getAmmoDmg should return a number')

        -- Mobs expose main weapon damage used by mobskills.
        local mobMain = mob:getWeaponDmg()
        assert(type(mobMain) == 'number', 'mob getWeaponDmg should return a number')
        assert(mobMain >= 1, 'mob getWeaponDmg should be at least 1 when a weapon is present')

        -- NPCs fall back to zero for all four helpers.
        assert(npc:getWeaponDmg() == 0, 'NPC getWeaponDmg should be zero')
        assert(npc:getOffhandDmg() == 0, 'NPC getOffhandDmg should be zero')
        assert(npc:getRangedDmg() == 0, 'NPC getRangedDmg should be zero')
        assert(npc:getAmmoDmg() == 0, 'NPC getAmmoDmg should be zero')

        -- Argument arity: helpers take no slot argument.
        assert(not pcall(player.getWeaponDmg), 'getWeaponDmg accepted missing self')
        assert(not pcall(player.getOffhandDmg), 'getOffhandDmg accepted missing self')
        assert(not pcall(player.getRangedDmg), 'getRangedDmg accepted missing self')
        assert(not pcall(player.getAmmoDmg), 'getAmmoDmg accepted missing self')
    end)
end)
