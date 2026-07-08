describe('Base entity job change helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WHM,
            level = 37,
        })
    end)

    it('updates player and mob jobs while unsupported entities are ignored', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local mobSubJob = mob:getSubJob()

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getMainJob() == xi.job.WHM, 'Player main job setup failed')
        assert(player:getMainLvl() == 37, 'Player main level setup failed')

        player:unlockJob(xi.job.RDM)
        player:changeJob(xi.job.RDM)
        assert(player:getMainJob() == xi.job.RDM, 'Player main job was not changed')
        assert(player:hasJob(xi.job.RDM) == true, 'Changed main job was not unlocked')
        assert(player:getMainLvl() == 37, 'Player main level changed with main job')
        assert(player:getJobLevel(xi.job.RDM) == 1, 'Changed main job stored level changed unexpectedly')

        player:unlockJob(xi.job.BLM)
        local oldSubLevel = player:getSubLvl()
        player:changesJob(xi.job.BLM)
        assert(player:getSubJob() == xi.job.BLM, 'Player support job was not changed')
        assert(player:hasJob(xi.job.BLM) == true, 'Changed support job was not unlocked')
        assert(player:getSubLvl() == oldSubLevel, 'Player support level changed with support job')

        mob:changeJob(xi.job.RDM)
        assert(mob:getMainJob() == xi.job.RDM, 'Mob main job was not changed')
        assert(mob:getSubJob() == mobSubJob, 'Mob support job changed unexpectedly')

        player:changeJob(0)
        player:changeJob(24)
        mob:changeJob(24)
        assert(player:getMainJob() == xi.job.RDM, 'Invalid player main job changed active job')
        assert(player:hasJob(0) == true, 'changeJob(0) did not preserve the JOB_NON unlock quirk')
        assert(mob:getMainJob() == xi.job.RDM, 'Invalid mob main job changed active job')

        mob:changesJob(xi.job.BLM)
        assert(mob:getSubJob() == mobSubJob, 'Mob support job changed through PC-only setter')

        player:changesJob(24)
        player:changesJob(280)
        assert(player:getSubJob() == xi.job.BLM, 'Invalid player support job changed active job')

        npc:changeJob(xi.job.BLM)
        npc:changesJob(xi.job.BLM)
        assert(npc:getMainJob() == 0, 'NPC main job fallback changed')
        assert(npc:getSubJob() == 0, 'NPC support job fallback changed')
    end)
end)
