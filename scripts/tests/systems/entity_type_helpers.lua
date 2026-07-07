describe('Base entity type helper bindings', function()
    ---@type CClientEntityPair
    local player

    ---@type CTestEntity
    local mob

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    after_each(function()
        if mob then
            mob:setAllegiance(0)
        end
    end)

    it('classifies real player, pet, trust, npc, and mob entities', function()
        player:setGMLevel(3)
        player:setVisibleGMLevel(3)

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')

        player:addSpell(xi.magic.spell.VALAINERAL)
        player.actions:useSpell(player, xi.magic.spell.VALAINERAL)
        xi.test.world:tickEntity(player)
        xi.test.world:skipTime(10)

        local trust = nil
        for _, member in ipairs(player:getPartyWithTrusts()) do
            if member:getName() == 'valaineral' then
                trust = member
                break
            end
        end

        assert(trust, 'Valaineral was not summoned')

        local npc = player.entities:get('Field_Manual')
        mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:isPC(), 'Player was not classified as PC')
        assert(not player:isNPC(), 'Player was incorrectly classified as NPC')
        assert(not player:isMob(), 'Player was incorrectly classified as mob')
        assert(not player:isPet(), 'Player was incorrectly classified as pet')
        assert(not player:isTrust(), 'Player was incorrectly classified as trust')
        assert(not player:isFellow(), 'Player was incorrectly classified as fellow')
        assert(not player:isAlly(), 'Player was incorrectly classified as ally mob')

        assert(pet:isPet(), 'Pet was not classified as pet')
        assert(not pet:isPC(), 'Pet was incorrectly classified as PC')
        assert(not pet:isMob(), 'Pet was incorrectly classified as mob')
        assert(not pet:isTrust(), 'Pet was incorrectly classified as trust')
        assert(not pet:isFellow(), 'Pet was incorrectly classified as fellow')
        assert(not pet:isAlly(), 'Pet was incorrectly classified as ally mob')

        assert(trust:isTrust(), 'Trust was not classified as trust')
        assert(not trust:isPC(), 'Trust was incorrectly classified as PC')
        assert(not trust:isPet(), 'Trust was incorrectly classified as pet')
        assert(not trust:isMob(), 'Trust was incorrectly classified as mob')
        assert(not trust:isFellow(), 'Trust was incorrectly classified as fellow')
        assert(not trust:isAlly(), 'Trust was incorrectly classified as ally mob')

        assert(npc:isNPC(), 'NPC was not classified as NPC')
        assert(not npc:isPC(), 'NPC was incorrectly classified as PC')
        assert(not npc:isMob(), 'NPC was incorrectly classified as mob')
        assert(not npc:isPet(), 'NPC was incorrectly classified as pet')
        assert(not npc:isTrust(), 'NPC was incorrectly classified as trust')
        assert(not npc:isFellow(), 'NPC was incorrectly classified as fellow')
        assert(not npc:isAlly(), 'NPC was incorrectly classified as ally mob')

        assert(mob:isMob(), 'Mob was not classified as mob')
        assert(not mob:isPC(), 'Mob was incorrectly classified as PC')
        assert(not mob:isNPC(), 'Mob was incorrectly classified as NPC')
        assert(not mob:isPet(), 'Mob was incorrectly classified as pet')
        assert(not mob:isTrust(), 'Mob was incorrectly classified as trust')
        assert(not mob:isFellow(), 'Mob was incorrectly classified as fellow')
        assert(not mob:isAlly(), 'Regular mob was incorrectly classified as ally mob')

        mob:setAllegiance(1)
        assert(mob:isAlly(), 'Player-allegiance mob was not classified as ally mob')
    end)
end)
