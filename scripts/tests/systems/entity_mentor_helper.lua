describe('Base entity mentor helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player mentor state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getMentor() == false, 'Player initial mentor flag was not false')
        assert(npc:getMentor() == false, 'NPC mentor fallback was not false')
        assert(mob:getMentor() == false, 'Mob mentor fallback was not false')

        player:setMentor(true)
        assert(player:getMentor() == true, 'Player mentor flag was not set')

        player:setMentor(false)
        assert(player:getMentor() == false, 'Player mentor flag was not cleared')

        npc:setMentor(false)
        mob:setMentor(false)
        assert(npc:getMentor() == false, 'NPC mentor fallback changed after setter')
        assert(mob:getMentor() == false, 'Mob mentor fallback changed after setter')
    end)
end)
