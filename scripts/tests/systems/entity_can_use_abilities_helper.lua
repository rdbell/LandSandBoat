describe('Base entity canUseAbilities helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('blocks battle entities with action-preventing effects and rejects NPCs', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        mob:spawn()

        assert(player:canUseAbilities(), 'player should use abilities without blockers')
        assert(mob:canUseAbilities(), 'mob should use abilities without blockers')

        local blockers =
        {
            xi.effect.SLEEP_I,
            xi.effect.IMPAIRMENT,
            xi.effect.SLEEP_II,
            xi.effect.STUN,
            xi.effect.AMNESIA,
            xi.effect.LULLABY,
            xi.effect.PETRIFICATION,
            xi.effect.TERROR,
        }

        for _, effect in ipairs(blockers) do
            mob:addStatusEffect(effect, { power = 1, duration = 60, origin = mob })
            assert(not mob:canUseAbilities(), string.format('mob should not use abilities with effect %u', effect))
            assert(mob:delStatusEffect(effect), string.format('effect %u should be removed', effect))
            assert(mob:canUseAbilities(), string.format('mob should use abilities after effect %u is removed', effect))
        end

        local npc = player.entities:moveTo('Aaveleon')
        assert(npc, 'Aaveleon NPC was not found')
        assert(not npc:canUseAbilities(), 'NPC should not use abilities')
        assert(not pcall(mob.canUseAbilities), 'canUseAbilities accepted missing self')
    end)
end)
