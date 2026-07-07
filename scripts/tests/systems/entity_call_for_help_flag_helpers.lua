describe('Base entity call-for-help flag helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates the mob call-for-help flag', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getCallForHelpFlag() == false, 'Player call-for-help fallback was not false')
        assert(npc:getCallForHelpFlag() == false, 'NPC call-for-help fallback was not false')
        assert(mob:getCallForHelpFlag() == false, 'Mob initial call-for-help flag was not false')

        player:setCallForHelpFlag(true)
        npc:setCallForHelpFlag(true)
        assert(player:getCallForHelpFlag() == false, 'Player call-for-help flag changed')
        assert(npc:getCallForHelpFlag() == false, 'NPC call-for-help flag changed')

        mob:setCallForHelpFlag(true)
        assert(mob:getCallForHelpFlag() == true, 'Mob call-for-help flag was not set')

        mob:setCallForHelpFlag(false)
        assert(mob:getCallForHelpFlag() == false, 'Mob call-for-help flag was not cleared')

        assert(not pcall(function()
            mob:setCallForHelpFlag('bad')
        end), 'setCallForHelpFlag accepted a non-boolean value')
    end)
end)
