describe('Patrolling mobs', function()
    it('does not cast buffs while patrolling', function()
        local player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
        local mob    = player.entities:moveTo('Wild_Rabbit')

        mob:respawn()
        mob:clearPath()
        mob:delStatusEffect(xi.effect.MINNE)
        mob:setSpellList(1) -- Knights Minne

        local mobPosition = mob:getPos()
        mob:pathThrough(
        {
            -- Keep the patrol active through the simulated controller tick.
            { x = mobPosition.x + 30, y = mobPosition.y, z = mobPosition.z },
            { x = mobPosition.x,     y = mobPosition.y, z = mobPosition.z },
        }, xi.path.flag.PATROL)

        xi.test.world:skipTime(1)

        assert(mob:isFollowingPath(), 'mob remains on its patrol path')
        assert(not mob:hasStatusEffect(xi.effect.MINNE), 'patrolling mob has not gained Minne')
    end)
end)
