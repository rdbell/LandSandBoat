describe('Base entity weapon base delay helpers', function()
    it('reads base melee/ranged delays with NPC and argument fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 75,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(player:getBaseDelay()) == 'number', 'getBaseDelay should return a number')
        assert(type(player:getBaseRangedDelay()) == 'number', 'getBaseRangedDelay should return a number')
        assert(type(player:getBaseWeaponDelay(xi.slot.MAIN)) == 'number',
            'getBaseWeaponDelay should return a number')

        -- Unequipped / default PC melee delay is the H2H base (480) or higher when geared.
        assert(player:getBaseDelay() >= 480, 'player getBaseDelay should be at least 480')
        assert(player:getBaseRangedDelay() >= 0, 'player getBaseRangedDelay should be non-negative')

        -- Mobs expose main-weapon base delay used by setDelay scripts.
        local mobDelay = mob:getBaseDelay()
        assert(type(mobDelay) == 'number', 'mob getBaseDelay should return a number')
        assert(mobDelay > 0, 'mob getBaseDelay should be positive when a weapon is present')
        assert(pcall(mob.setDelay, mob, 240), 'setDelay rejected a valid mob update')
        -- setDelay mutates m_delay; base delay observability is loader-dependent.
        assert(type(mob:getBaseDelay()) == 'number', 'mob getBaseDelay remains numeric after setDelay')

        -- NPCs are not battle entities for base delay helpers.
        assert(npc:getBaseDelay() == 0, 'NPC getBaseDelay should be zero')
        assert(npc:getBaseRangedDelay() == 0, 'NPC getBaseRangedDelay should be zero')
        assert(npc:getBaseWeaponDelay(xi.slot.MAIN) == 0, 'NPC getBaseWeaponDelay should be zero')

        -- getBaseWeaponDelay is PC-only.
        assert(mob:getBaseWeaponDelay(xi.slot.MAIN) == 0, 'mob getBaseWeaponDelay should be zero')

        assert(not pcall(player.getBaseDelay), 'getBaseDelay accepted missing self')
        assert(not pcall(player.getBaseRangedDelay), 'getBaseRangedDelay accepted missing self')
        assert(not pcall(player.getBaseWeaponDelay), 'getBaseWeaponDelay accepted missing self')
        assert(not pcall(player.getBaseWeaponDelay, player), 'getBaseWeaponDelay accepted missing slot')
        assert(not pcall(player.getBaseWeaponDelay, player, 'bad'),
            'getBaseWeaponDelay accepted non-numeric slot')
    end)
end)
