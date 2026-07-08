describe('Base entity target helper binding', function()
    ---@type CClientEntityPair
    local player
    ---@type CTestEntity
    local mob

    local function waitForTarget(entity, expectedTargID, message)
        local target = nil

        for _ = 1, 5 do
            xi.test.world:tickEntity(entity)
            target = entity:getTarget()

            if target ~= nil and target:getTargID() == expectedTargID then
                return target
            end

            xi.test.world:skipTime(1)
        end

        assert(target ~= nil, message)
        assert(target:getTargID() == expectedTargID, message)
    end

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        mob = nil
    end)

    after_each(function()
        if player then
            local pet = player:getPet()

            if pet then
                player:despawnPet()
            end
        end

        if mob then
            mob:disengage()
            mob:respawn()
        end
    end)

    it('returns battle targets and falls back for entities without one', function()
        mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:respawn()
        local pos = player:getPos()
        mob:setPos(pos.x, pos.y, pos.z)

        assert(player:getTarget() == nil, 'player should start without a target')
        assert(mob:getTarget() == nil, 'mob should start without a target')
        assert(npc:getTarget() == nil, 'NPC getTarget should fall back to nil')

        mob:updateEnmity(player)
        waitForTarget(mob, player:getTargID(), 'mob target should be the player')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(pet:getTarget() == nil, 'idle pet should not have a target')
        pet:engage(mob:getTargID())
        waitForTarget(pet, mob:getTargID(), 'pet target should be the engaged mob')

        assert(not pcall(player.getTarget), 'getTarget accepted missing self')
    end)
end)
