describe('Base entity call-for-help blocked helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates the mob call-for-help blocked flag', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getCallForHelpBlocked() == false, 'Player call-for-help blocked fallback was not false')
        assert(npc:getCallForHelpBlocked() == false, 'NPC call-for-help blocked fallback was not false')
        assert(mob:getCallForHelpBlocked() == false, 'Mob initial call-for-help blocked flag was not false')

        player:setCallForHelpBlocked(true)
        npc:setCallForHelpBlocked(true)
        assert(player:getCallForHelpBlocked() == false, 'Player call-for-help blocked flag changed')
        assert(npc:getCallForHelpBlocked() == false, 'NPC call-for-help blocked flag changed')

        mob:setCallForHelpBlocked(true)
        assert(mob:getCallForHelpBlocked() == true, 'Mob call-for-help blocked flag was not set')

        mob:setCallForHelpBlocked(false)
        assert(mob:getCallForHelpBlocked() == false, 'Mob call-for-help blocked flag was not cleared')

        assert(not pcall(function()
            mob:setCallForHelpBlocked('bad')
        end), 'setCallForHelpBlocked accepted a non-boolean value')
    end)
end)
