describe('Base entity setPet and getMaster helper bindings', function()
    it('links and clears battle entity pet/master relationships', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        assert(player:getPet() == nil, 'player should not start with a pet')
        assert(mob:getMaster() == nil, 'mob should not start with a master')
        assert(npc:getMaster() == nil, 'NPC should fall back to nil master')

        player:setPet(mob)
        local assignedPet = player:getPet()
        assert(assignedPet ~= nil, 'player should return assigned pet')
        assert(assignedPet:getTargID() == mob:getTargID(), 'player should return assigned pet target ID')

        local assignedMaster = mob:getMaster()
        assert(assignedMaster ~= nil, 'assigned pet should return a master')
        assert(assignedMaster:getName() == player:getName(), 'assigned pet should return player master')

        player:setPet(nil)
        assert(player:getPet() == nil, 'cleared pet should return nil')
        assert(mob:getMaster() == nil, 'cleared pet should lose master')

        player:setPet(mob)
        player:setPet({})
        assert(player:getPet() == nil, 'invalid pet argument should clear current pet')
        assert(mob:getMaster() == nil, 'invalid pet argument should clear current pet master')

        npc:setPet(mob)
        assert(npc:getPet() == nil, 'NPC setPet should be ignored')

        assert(not pcall(player.setPet), 'setPet accepted missing self')
        assert(not pcall(player.getMaster), 'getMaster accepted missing self')
    end)
end)
