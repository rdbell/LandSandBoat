describe('Base entity job and level helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WHM,
            level = 37,
        })
    end)

    it('reads battle job and level fields while NPCs fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getMainJob() == xi.job.WHM, 'Player main job was not WHM')
        assert(player:getMainLvl() == 37, 'Player main level was not 37')
        assert(player:getJobLevel(xi.job.WHM) == 37, 'Player WHM job level was not 37')
        assert(player:getJobLevel(24) == 0, 'Invalid job ID did not fall back to zero')
        assert(player:getJobLevel(280) == 0, 'Wrapped invalid job ID did not fall back to zero')

        assert(npc:getMainJob() == 0, 'NPC main job fallback was not zero')
        assert(npc:getSubJob() == 0, 'NPC sub job fallback was not zero')
        assert(npc:getMainLvl() == 0, 'NPC main level fallback was not zero')
        assert(npc:getSubLvl() == 0, 'NPC sub level fallback was not zero')
        assert(npc:getJobLevel(xi.job.WHM) == 0, 'NPC job level fallback was not zero')

        assert(mob:getMainJob() > 0, 'Mob main job was not populated')
        assert(mob:getSubJob() >= 0, 'Mob sub job was not readable')
        assert(mob:getMainLvl() >= 0, 'Mob main level was not readable')
        assert(mob:getSubLvl() >= 0, 'Mob sub level was not readable')
        assert(mob:getJobLevel(xi.job.WHM) == 0, 'Mob job level fallback was not zero')
    end)

    it('checks and unlocks player jobs', function()
        assert(player:hasJob(xi.job.WHM) == true, 'Current job was not unlocked')
        assert(player:hasJob(24) == false, 'Invalid job ID did not return false')
        assert(player:hasJob(280) == false, 'Wrapped invalid job ID did not return false')

        player:unlockJob(xi.job.RUN)
        player:unlockJob(24)
        player:unlockJob(280)

        assert(player:hasJob(xi.job.RUN) == true, 'Unlocked RUN was not reported as unlocked')
        assert(player:getJobLevel(xi.job.RUN) == 1, 'Unlocked RUN did not receive level 1')

        player:unlockJob(0)

        assert(player:hasJob(0) == true, 'unlockJob(0) did not set the JOB_NON bit')
        assert(player:getJobLevel(xi.job.WAR) >= 1, 'unlockJob(0) did not preserve WAR level fallback')
    end)
end)
