describe('Base entity key item helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player key item state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local keyItem = xi.ki.TENSHODO_MEMBERS_CARD
        local otherTableKeyItem = xi.ki.MOGHANCEMENT_FIRE

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:hasKeyItem(keyItem) == false, 'Initial key item was unexpectedly owned')
        assert(player:seenKeyItem(keyItem) == false, 'Initial key item was unexpectedly seen')

        player:addKeyItem(keyItem)
        assert(player:hasKeyItem(keyItem) == true, 'addKeyItem did not set key item')
        assert(player:seenKeyItem(keyItem) == false, 'addKeyItem unexpectedly marked key item seen')

        player:addKeyItem(keyItem)
        assert(player:hasKeyItem(keyItem) == true, 'Repeated addKeyItem cleared key item')

        player:unseenKeyItem(keyItem)
        assert(player:seenKeyItem(keyItem) == false, 'unseenKeyItem marked unseen key item seen')
        assert(player:hasKeyItem(keyItem) == true, 'unseenKeyItem cleared owned key item')

        player:delKeyItem(keyItem)
        assert(player:hasKeyItem(keyItem) == false, 'delKeyItem did not clear key item')
        assert(player:seenKeyItem(keyItem) == false, 'delKeyItem marked key item seen')

        player:addKeyItem(otherTableKeyItem)
        assert(player:hasKeyItem(otherTableKeyItem) == true, 'addKeyItem did not set key item in another table')
        player:delKeyItem(otherTableKeyItem)
        assert(player:hasKeyItem(otherTableKeyItem) == false, 'delKeyItem did not clear key item in another table')

        player:addKeyItem(4096)
        assert(player:hasKeyItem(4096) == false, 'Invalid key item table became readable')

        player:addKeyItem(65537)
        assert(player:hasKeyItem(1) == true, 'Key item ID did not wrap through uint16 input')
        player:delKeyItem(65537)
        assert(player:hasKeyItem(1) == false, 'Wrapped key item delete did not clear bit 1')

        npc:addKeyItem(keyItem)
        mob:addKeyItem(keyItem)
        npc:unseenKeyItem(keyItem)
        mob:unseenKeyItem(keyItem)
        npc:delKeyItem(keyItem)
        mob:delKeyItem(keyItem)
        assert(npc:hasKeyItem(keyItem) == false, 'NPC key item fallback changed')
        assert(mob:hasKeyItem(keyItem) == false, 'Mob key item fallback changed')
        assert(npc:seenKeyItem(keyItem) == false, 'NPC seen key item fallback changed')
        assert(mob:seenKeyItem(keyItem) == false, 'Mob seen key item fallback changed')
    end)
end)
