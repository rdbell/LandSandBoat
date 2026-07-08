describe('Base entity level setter helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WHM,
            sjob  = xi.job.RDM,
            level = 37,
        })
    end)

    it('updates player main and support levels while non-players are ignored', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        local mobMainLevel = mob:getMainLvl()

        player:setLevel(55)
        assert(player:getMainLvl() == 55, 'Player main level was not updated')
        assert(player:getJobLevel(xi.job.WHM) == 55, 'Player main job level was not updated')

        player:changesJob(xi.job.RDM)
        player:setsLevel(99)
        assert(player:getSubLvl() == 27, 'Player support level was not capped to half main level')
        assert(player:getJobLevel(xi.job.RDM) == 99, 'Player support job level was not updated')

        player:setLevel(100)
        player:setsLevel(100)
        assert(player:getMainLvl() == 55, 'Invalid player main level was not ignored')
        assert(player:getSubLvl() == 27, 'Invalid player support level was not ignored')

        player:setLevel(0)
        assert(player:getMainLvl() == 1, 'Player main level zero did not clamp to one')
        assert(player:getJobLevel(xi.job.WHM) == 0, 'Player stored main job level did not preserve zero')
        assert(player:getSubLvl() == 1, 'Player support level was not recapped after main level zero clamp')

        player:setLevel(280)
        assert(player:getMainLvl() == 24, 'Player main level did not wrap uint8 input')
        assert(player:getSubLvl() == 12, 'Player support level was not recapped after main level update')

        npc:setLevel(75)
        npc:setsLevel(75)
        mob:setLevel(75)
        mob:setsLevel(75)
        assert(npc:getMainLvl() == 0, 'NPC main level fallback changed after setter')
        assert(npc:getSubLvl() == 0, 'NPC support level fallback changed after setter')
        assert(mob:getMainLvl() == mobMainLevel, 'Mob main level changed after setter')
    end)
end)
