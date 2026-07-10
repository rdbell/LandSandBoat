describe('Base entity main-weapon item-level helpers', function()
    it('reads main-slot iLvl bonuses with NPC fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(player:getILvlMacc()) == 'number', 'getILvlMacc should return a number')
        assert(type(player:getILvlSkill()) == 'number', 'getILvlSkill should return a number')
        assert(type(player:getILvlParry()) == 'number', 'getILvlParry should return a number')
        assert(player:getILvlMacc() >= 0, 'getILvlMacc should be non-negative')
        assert(player:getILvlSkill() >= 0, 'getILvlSkill should be non-negative')
        assert(player:getILvlParry() >= 0, 'getILvlParry should be non-negative')

        assert(type(mob:getILvlMacc()) == 'number', 'mob getILvlMacc should return a number')

        -- NPCs are not battle entities for these helpers.
        assert(npc:getILvlMacc() == 0, 'NPC getILvlMacc should be zero')
        assert(npc:getILvlSkill() == 0, 'NPC getILvlSkill should be zero')
        assert(npc:getILvlParry() == 0, 'NPC getILvlParry should be zero')

        assert(not pcall(player.getILvlMacc), 'getILvlMacc accepted missing self')
        assert(not pcall(player.getILvlSkill), 'getILvlSkill accepted missing self')
        assert(not pcall(player.getILvlParry), 'getILvlParry accepted missing self')
    end)
end)
