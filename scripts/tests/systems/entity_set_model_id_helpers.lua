describe('Base entity set model ID helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('updates base model IDs for non-player entities and look slots for slotted calls', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        mob:setModelId(0x0456)
        assert(mob:getModelId() == 0x0456, 'Mob base model ID was not updated')

        npc:setModelId(0x0123)
        assert(npc:getModelId() == 0x0123, 'NPC base model ID was not updated')

        npc:setModelId(0x0047, xi.slot.BODY)
        assert(npc:getModelId() == 0x0123, 'Slotted NPC update changed base model ID')

        npc:setModelId(0x0888, 'not a slot')
        assert(npc:getModelId() == 0x0888, 'NPC nonnumeric slot did not fall back to base model ID update')

        player:setModelId(0x1001)
        player:setModelId(0x2002, xi.slot.BODY)
        player:setModelId(0x3003, xi.slot.HANDS)
        player:setModelId(0x4004, xi.slot.BACK)
        player:setModelId(0x5005, 'not a slot')

        local equipment = player:getEquipmentModelIds()
        assert(equipment.main == 0x5005, 'Player nonnumeric slot did not default to main slot')
        assert(equipment.body == 0x2002, 'Player body slot model ID was not updated')
        assert(equipment.hands == 0x3003, 'Player hands slot model ID was not updated')
        assert(equipment.head == 0, 'Unsupported player slot update changed head slot')
        assert(player:getModelId() == 0x0100, 'Player base model ID changed after slot updates')
    end)
end)
