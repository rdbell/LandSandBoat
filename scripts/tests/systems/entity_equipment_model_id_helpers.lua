describe('Base entity equipment model ID helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('returns visible player equipment model IDs and zeroes for non-player entities', function()
        local npc = player.entities:get('Field_Manual')

        assert(npc, 'Field Manual NPC was not found')

        player:setModelId(0x1001)
        player:setModelId(0x2002, xi.slot.BODY)
        player:setModelId(0x3003, xi.slot.HANDS)
        player:setModelId(0x4004, xi.slot.HEAD)
        player:setModelId(0x5005, xi.slot.SUB)
        player:setModelId(0x6006, xi.slot.RANGED)
        player:setModelId(0x7007, xi.slot.LEGS)
        player:setModelId(0x8008, xi.slot.FEET)

        local equipment = player:getEquipmentModelIds()
        assert(equipment.head == 0x4004, 'Player head model ID was not returned')
        assert(equipment.body == 0x2002, 'Player body model ID was not returned')
        assert(equipment.hands == 0x3003, 'Player hands model ID was not returned')
        assert(equipment.main == 0x1001, 'Player main model ID was not returned')
        assert(equipment.sub == 0x5005, 'Player sub model ID was not returned')
        assert(equipment.ranged == nil, 'Unexpected ranged key was returned')
        assert(equipment.legs == nil, 'Unexpected legs key was returned')
        assert(equipment.feet == nil, 'Unexpected feet key was returned')

        npc:setModelId(0x0047, xi.slot.BODY)

        local npcEquipment = npc:getEquipmentModelIds()
        assert(npcEquipment.head == 0, 'NPC head model ID fallback was not zero')
        assert(npcEquipment.body == 0, 'NPC body model ID fallback was not zero')
        assert(npcEquipment.hands == 0, 'NPC hands model ID fallback was not zero')
        assert(npcEquipment.main == 0, 'NPC main model ID fallback was not zero')
        assert(npcEquipment.sub == 0, 'NPC sub model ID fallback was not zero')
    end)
end)
