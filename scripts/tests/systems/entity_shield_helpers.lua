describe('Base entity shield size/defense helpers', function()
    it('reads PC shield equip and trust shield size with NPC fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        -- Unarmed / no shield: size and defense are zero.
        assert(type(player:getShieldSize()) == 'number', 'getShieldSize should return a number')
        assert(type(player:getShieldDefense()) == 'number', 'getShieldDefense should return a number')
        assert(player:getShieldSize() == 0, 'unarmed getShieldSize should be zero')
        assert(player:getShieldDefense() == 0, 'unarmed getShieldDefense should be zero')

        -- Mobs and NPCs are not PC/Trust for getShieldSize; defense is PC-only.
        assert(type(mob:getShieldSize()) == 'number', 'mob getShieldSize should return a number')
        assert(mob:getShieldSize() == 0, 'mob getShieldSize should be zero')
        assert(mob:getShieldDefense() == 0, 'mob getShieldDefense should be zero')
        assert(npc:getShieldSize() == 0, 'NPC getShieldSize should be zero')
        assert(npc:getShieldDefense() == 0, 'NPC getShieldDefense should be zero')

        assert(not pcall(player.getShieldSize), 'getShieldSize accepted missing self')
        assert(not pcall(player.getShieldDefense), 'getShieldDefense accepted missing self')
    end)
end)
