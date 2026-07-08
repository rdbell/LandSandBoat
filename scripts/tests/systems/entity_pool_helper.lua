describe('Base entity pool helper binding', function()
    it('reads mob and trust pool IDs while invalid receivers fall back', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        player:addSpell(xi.magic.spell.VALAINERAL)
        player.actions:useSpell(player, xi.magic.spell.VALAINERAL)
        xi.test.world:tickEntity(player)
        xi.test.world:skipTime(10)

        local trust = nil
        for _, member in ipairs(player:getPartyWithTrusts()) do
            if member:getName() == 'valaineral' then
                trust = member
                break
            end
        end

        assert(trust, 'Valaineral was not summoned')

        assert(mob:getPool() == 4343, 'expected Wild Rabbit pool to be 4343')
        assert(trust:getPool() == 5910, 'expected Valaineral trust pool to be 5910')
        assert(npc:getPool() == 0, 'NPC getPool should fall back to zero')
        assert(player:getPool() == 0, 'player getPool should fall back to zero')

        assert(not pcall(mob.getPool), 'getPool accepted missing self')
    end)
end)
