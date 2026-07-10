describe('Base entity modifier helper bindings', function()
    it('sets signed modifiers and ignores NPC and zero-ID updates', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:setMod(xi.mod.ATT, 321)
        assert(mob:getMod(xi.mod.ATT) == 321, 'setMod did not store a positive value')
        mob:setMod(xi.mod.ATT, -32768)
        assert(mob:getMod(xi.mod.ATT) == -32768, 'setMod did not store the minimum int16 value')
        mob:setMod(65535, 32767)
        assert(mob:getMod(65535) == 32767, 'setMod did not store maximum uint16/int16 values')
        mob:setMod(0, 99)
        assert(mob:getMod(0) == 0, 'setMod should ignore modifier ID zero')

        npc:setMod(xi.mod.ATT, 100)
        assert(npc:getMod(xi.mod.ATT) == 0, 'NPC setMod should be ignored')

        assert(not pcall(mob.setMod), 'setMod accepted missing self')
        assert(not pcall(mob.setMod, mob), 'setMod accepted missing modifier ID')
        assert(not pcall(mob.setMod, mob, xi.mod.ATT), 'setMod accepted missing value')
        assert(not pcall(mob.setMod, mob, 'bad', 1), 'setMod accepted non-numeric modifier ID')
        assert(not pcall(mob.setMod, mob, xi.mod.ATT, 'bad'), 'setMod accepted non-numeric value')
    end)
end)
