describe('Base entity title helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player title ownership while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getTitle() == 0, 'Initial player title was not zero')
        assert(player:hasTitle(42) == false, 'Player unexpectedly started with title 42')

        player:addTitle(42)
        assert(player:getTitle() == 42, 'addTitle did not update current title')
        assert(player:hasTitle(42) == true, 'addTitle did not add title ownership')

        player:setTitle(43)
        assert(player:getTitle() == 43, 'setTitle did not update current title')
        assert(player:hasTitle(42) == true, 'setTitle removed existing title ownership')
        assert(player:hasTitle(43) == true, 'setTitle did not add title ownership')

        player:delTitle(43)
        assert(player:getTitle() == 0, 'delTitle did not clear current title')
        assert(player:hasTitle(43) == false, 'delTitle did not remove title ownership')
        assert(player:hasTitle(42) == true, 'delTitle removed unrelated title ownership')

        player:setTitle(65537)
        assert(player:getTitle() == 1, 'setTitle did not wrap uint16 input')
        assert(player:hasTitle(1) == true, 'Wrapped title ownership was not added')

        player:setTitle(1144)
        assert(player:getTitle() == 1144, 'Out-of-range title was not made current')
        assert(player:hasTitle(1144) == false, 'Out-of-range title ownership was unexpectedly added')
        player:delTitle(1144)
        assert(player:getTitle() == 1144, 'Deleting out-of-range title unexpectedly cleared current title')

        npc:addTitle(42)
        npc:setTitle(43)
        npc:delTitle(43)
        mob:addTitle(42)
        mob:setTitle(43)
        mob:delTitle(43)
        assert(npc:getTitle() == 0, 'NPC title fallback changed after setters')
        assert(mob:getTitle() == 0, 'Mob title fallback changed after setters')
        assert(npc:hasTitle(42) == false, 'NPC title ownership fallback changed')
        assert(mob:hasTitle(42) == false, 'Mob title ownership fallback changed')
    end)
end)
