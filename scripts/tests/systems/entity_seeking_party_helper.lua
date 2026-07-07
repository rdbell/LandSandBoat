describe('Base entity seeking party helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads player seeking-party state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:isSeekingParty() == false, 'Player initial seeking-party flag was not false')
        assert(npc:isSeekingParty() == false, 'NPC seeking-party fallback was not false')
        assert(mob:isSeekingParty() == false, 'Mob seeking-party fallback was not false')
    end)
end)
