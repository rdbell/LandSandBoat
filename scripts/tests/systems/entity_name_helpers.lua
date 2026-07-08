describe('Base entity name helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads internal and packet names and refuses player renames', function()
        local playerName       = player:getName()
        local playerPacketName = player:getPacketName()

        assert(playerName ~= '', 'Player name was empty')

        player:renameEntity('Ignored Player Name', true)

        assert(player:getName() == playerName, 'Player internal name changed')
        assert(player:getPacketName() == playerPacketName, 'Player packet name changed')

        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')

        local mobName       = mob:getName()
        local mobPacketName = mob:getPacketName()

        assert(mobName == 'Wild_Rabbit', string.format('Unexpected mob name: %s', mobName))
        assert(mobPacketName ~= '', 'Mob packet name was empty')

        local npc = player.entities:get('Field_Manual')
        assert(npc, 'Field Manual NPC was not found')

        assert(pcall(player.hideName, player, true), 'player hideName rejected a valid hide')
        assert(pcall(player.hideName, player, false), 'player hideName rejected a valid show')
        assert(pcall(npc.hideName, npc, true), 'NPC hideName rejected a valid hide')
        assert(pcall(npc.hideName, npc, false), 'NPC hideName rejected a valid show')
        assert(pcall(mob.hideName, mob, true), 'mob hideName rejected a valid hide')
        assert(pcall(mob.hideName, mob, false), 'mob hideName rejected a valid show')

        assert(not pcall(mob.hideName), 'hideName accepted missing self')
        assert(not pcall(mob.hideName, mob), 'hideName accepted missing state')
        assert(not pcall(mob.hideName, mob, 'bad'), 'hideName accepted non-boolean state')
    end)
end)
