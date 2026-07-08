describe('Base entity battle state predicate bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reports battle state and spawn predicates for players and mobs', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        mob:spawn()

        assert(player:isAlive(), 'player should start alive')
        assert(not player:isDead(), 'player should not start dead')
        assert(not player:isEngaged(), 'player should not start engaged')
        assert(not player:isCharmed(), 'player should not start charmed')

        assert(mob:isAlive(), 'mob should start alive')
        assert(not mob:isDead(), 'mob should not start dead')
        assert(not mob:isEngaged(), 'mob should not start engaged')
        assert(not mob:isCharmed(), 'mob should not start charmed')
        assert(mob:isSpawned(), 'mob should start spawned')

        mob:setHP(0)
        assert(not mob:isAlive(), 'mob should not be alive at zero HP')
        assert(mob:isDead(), 'mob should be dead at zero HP')

        mob:spawn()
        assert(mob:isSpawned(), 'spawn should make mob spawned')
        mob:despawn()
        assert(not mob:isSpawned(), 'despawn should make mob unspawned')
        assert(not mob:isAlive(), 'despawned mob should not be alive')
        assert(mob:isDead(), 'despawned mob should be dead')

        assert(not player:isSpawned(), 'player is not a spawnable mob or NPC')
    end)
end)
