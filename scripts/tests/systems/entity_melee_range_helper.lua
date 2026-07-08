describe('Base entity melee range helper binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('computes battle entity melee ranges from hitbox sizes', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        player:setHitboxSize(0.5)
        mob:setHitboxSize(1.25)

        assert(player:getMeleeRange(mob) == 3.75, 'expected player-to-mob melee range')
        assert(mob:getMeleeRange(player) == 3.75, 'expected mob-to-player melee range')
        assert(mob:getMeleeRange(mob) == 4.5, 'expected self melee range')
        assert(player:getMeleeRange(npc) == 0, 'expected NPC target to fall back to zero')
        assert(npc:getMeleeRange(player) == 0, 'expected NPC source to fall back to zero')

        assert(not pcall(mob.getMeleeRange), 'getMeleeRange accepted missing self')
        assert(not pcall(mob.getMeleeRange, mob), 'getMeleeRange accepted missing target')
        assert(not pcall(mob.getMeleeRange, mob, 'bad'), 'getMeleeRange accepted non-entity target')
    end)
end)
