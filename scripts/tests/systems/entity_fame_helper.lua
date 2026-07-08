describe('Base entity fame helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player fame while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local sandyLog = { fame_area = xi.fameArea.SANDORIA }

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getFame(xi.fameArea.BASTOK) == 0, 'Initial Bastok fame was not zero')
        assert(player:getFameLevel(xi.fameArea.BASTOK) == 1, 'Initial Bastok fame level was not one')

        player:addFame(xi.fameArea.BASTOK, 49)
        assert(player:getFame(xi.fameArea.BASTOK) == 49, 'addFame did not add direct fame')
        assert(player:getFameLevel(xi.fameArea.BASTOK) == 1, 'Fame level advanced before threshold')

        player:addFame(xi.fameArea.BASTOK, 1)
        assert(player:getFame(xi.fameArea.BASTOK) == 50, 'addFame did not accumulate direct fame')
        assert(player:getFameLevel(xi.fameArea.BASTOK) == 2, 'Fame level did not advance at threshold')

        player:setFame(sandyLog, 100)
        assert(player:getFame(sandyLog) == 100, 'setFame did not accept table fame area')

        player:setFame(xi.fameArea.WINDURST, 200)
        player:setFame(xi.fameArea.JEUNO, 300)
        assert(player:getFame(xi.fameArea.JEUNO) == 416, 'Jeuno fame did not include city average')

        player:setFame(xi.fameArea.SELBINA_RABAO, 80)
        assert(player:getFame(xi.fameArea.SANDORIA) == 80, 'Selbina/Rabao did not update Sandoria fame')
        assert(player:getFame(xi.fameArea.BASTOK) == 80, 'Selbina/Rabao did not update Bastok fame')
        assert(player:getFame(xi.fameArea.SELBINA_RABAO) == 80, 'Selbina/Rabao derived fame was wrong')

        player:setFame(xi.fameArea.ABYSSEA_KONSCHTAT, 613)
        assert(player:getFame(xi.fameArea.ABYSSEA_KONSCHTAT) == 613, 'Abyssea fame was not set')
        assert(player:getFameLevel(xi.fameArea.ABYSSEA_KONSCHTAT) == 6, 'Abyssea fame level did not cap at six')

        player:setFame(xi.fameArea.ADOULIN, 550)
        player:addFame(xi.fameArea.ADOULIN, 65537)
        assert(player:getFame(xi.fameArea.ADOULIN) == 551, 'addFame did not wrap uint16 fame input')
        assert(player:getFameLevel(xi.fameArea.ADOULIN) == 8, 'Adoulin fame level was wrong')

        player:addFame(16, 500)
        player:setFame(16, 500)
        assert(player:getFame(16) == 0, 'Invalid fame area did not read as zero')
        assert(player:getFameLevel(16) == 1, 'Invalid fame area did not keep baseline fame level')

        npc:addFame(xi.fameArea.BASTOK, 50)
        npc:setFame(xi.fameArea.BASTOK, 50)
        mob:addFame(xi.fameArea.BASTOK, 50)
        mob:setFame(xi.fameArea.BASTOK, 50)
        assert(npc:getFame(xi.fameArea.BASTOK) == 0, 'NPC fame fallback changed after setters')
        assert(mob:getFame(xi.fameArea.BASTOK) == 0, 'Mob fame fallback changed after setters')
        assert(npc:getFameLevel(xi.fameArea.BASTOK) == 0, 'NPC fame level fallback changed')
        assert(mob:getFameLevel(xi.fameArea.BASTOK) == 0, 'Mob fame level fallback changed')
    end)
end)
