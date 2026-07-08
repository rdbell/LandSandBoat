describe('Base entity level restriction helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WHM,
            level = 55,
        })
    end)

    it('updates player level restriction while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local mobMainLevel = mob:getMainLvl()

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        player:changesJob(xi.job.RDM)
        player:setsLevel(99)
        assert(player:getMainLvl() == 55, 'Player main level setup failed')
        assert(player:getSubLvl() == 27, 'Player support level setup was not capped')

        assert(player:levelRestriction(30) == 30, 'Player level restriction was not set')
        assert(player:getMainLvl() == 30, 'Player main level was not restricted')
        assert(player:getSubLvl() == 15, 'Player support level was not recapped by restriction')

        assert(player:levelRestriction() == 30, 'Missing level did not return current restriction')
        assert(player:levelRestriction(nil) == 30, 'Nil level did not return current restriction')
        assert(player:levelRestriction('bad') == 30, 'Non-numeric level did not return current restriction')

        assert(player:levelRestriction(80) == 80, 'Above-current restriction value was not stored')
        assert(player:getMainLvl() == 55, 'Above-current restriction did not restore main level')
        assert(player:getSubLvl() == 27, 'Above-current restriction did not restore support level')

        assert(player:levelRestriction(0) == 0, 'Zero restriction did not clear stored restriction')
        assert(player:getMainLvl() == 55, 'Clearing restriction did not preserve main level')
        assert(player:getSubLvl() == 27, 'Clearing restriction did not preserve support level')

        assert(npc:levelRestriction(30) == 0, 'NPC level restriction fallback was not zero')
        assert(mob:levelRestriction(30) == 0, 'Mob level restriction fallback was not zero')
        assert(npc:getMainLvl() == 0, 'NPC main level fallback changed after restriction')
        assert(mob:getMainLvl() == mobMainLevel, 'Mob main level changed after restriction')
    end)
end)
