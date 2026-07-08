describe('Base entity prevent action effect helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reports action-preventing effects while NPCs fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')
        mob:spawn()

        assert(not player:hasPreventActionEffect(), 'player should not be blocked without effects')
        assert(not mob:hasPreventActionEffect(), 'mob should not be blocked without effects')

        local blockers =
        {
            xi.effect.SLEEP_I,
            xi.effect.SLEEP_II,
            xi.effect.PETRIFICATION,
            xi.effect.LULLABY,
            xi.effect.CHARM_I,
            xi.effect.CHARM_II,
            xi.effect.PENALTY,
            xi.effect.STUN,
            xi.effect.TERROR,
        }

        for _, effect in ipairs(blockers) do
            mob:addStatusEffect(effect, { power = 1, duration = 60, origin = mob })
            assert(mob:hasPreventActionEffect(), string.format('mob should be blocked with effect %u', effect))
            assert(mob:hasPreventActionEffect(true), string.format('extra args should not change effect %u', effect))
            assert(mob:delStatusEffect(effect), string.format('effect %u should be removed', effect))
            assert(not mob:hasPreventActionEffect(), string.format('mob should not be blocked after effect %u is removed', effect))
        end

        local npc = player.entities:moveTo('Aaveleon')
        assert(npc, 'Aaveleon NPC was not found')
        assert(not npc:hasPreventActionEffect(), 'NPC should fall back to false')
        assert(not pcall(mob.hasPreventActionEffect), 'hasPreventActionEffect accepted missing self')
    end)
end)
