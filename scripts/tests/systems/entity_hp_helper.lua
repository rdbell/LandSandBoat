describe('Base entity HP helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates battle entity HP while NPCs fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')
        mob:spawn()

        mob:setMaxHP(1000)
        assert(mob:getMaxHP() == 1000, 'setMaxHP did not update mob max HP')
        assert(mob:getBaseHP() == 1000, 'getBaseHP did not report raw mob max HP')

        mob:setHP(333)
        assert(mob:getHP() == 333, 'setHP did not update mob HP')
        assert(mob:getHPP() == 33, 'getHPP should floor the mob HP percent')

        assert(mob:addHP(200) == 200, 'addHP did not report restored mob HP')
        assert(mob:getHP() == 533, 'addHP did not restore mob HP')

        assert(mob:addHP(9999) == 467, 'addHP did not report capped restore')
        assert(mob:getHP() == 1000, 'addHP did not cap mob HP')
        assert(mob:getHPP() == 100, 'capped mob HP should be 100 percent')

        mob:setHP(700)
        assert(mob:restoreHP(500) == 300, 'restoreHP did not report capped restore')
        assert(mob:getHP() == 1000, 'restoreHP did not cap at max HP')

        mob:delHP(275)
        assert(mob:getHP() == 725, 'delHP did not subtract mob HP')

        mob:setHP(25)
        mob:delHP(999)
        assert(mob:getHP() == 0, 'delHP did not floor mob HP')
        assert(mob:getHPP() == 0, 'zero HP should report zero HPP')
        assert(mob:restoreHP(40) == 40, 'restoreHP did not revive from zero HP')
        assert(mob:getHP() == 40, 'restoreHP did not update zero HP')
        assert(mob:getHPP() == 4, 'low nonzero HPP should floor to 4 percent')

        mob:setHP(500)
        mob:setMaxHP(300)
        assert(mob:getMaxHP() == 300, 'setMaxHP did not lower mob max HP')
        assert(mob:getHP() == 300, 'setMaxHP did not clamp current HP')
        mob:setMaxHP(0)
        assert(mob:getMaxHP() == 1, 'setMaxHP should floor max HP at one')
        assert(mob:getHP() == 1, 'setMaxHP floor should clamp current HP to one')

        local sleepEffects =
        {
            xi.effect.SLEEP_I,
            xi.effect.SLEEP_II,
            xi.effect.LULLABY,
        }

        for _, effect in ipairs(sleepEffects) do
            mob:setMaxHP(1000)
            mob:setHP(500)
            mob:addStatusEffect(effect, { power = 1, duration = 60, origin = mob })
            assert(mob:hasStatusEffect(effect), string.format('effect %u was not applied', effect))
            assert(mob:addHP(1) == 1, 'addHP did not report one restored HP')
            assert(not mob:hasStatusEffect(effect), string.format('addHP should remove effect %u', effect))
        end

        mob:setHP(500)
        mob:setUnkillable(true)
        mob:delHP(999)
        assert(mob:getHP() == 1, 'unkillable mob delHP should floor at one')
        mob:setHP(0)
        assert(mob:getHP() == 1, 'unkillable mob setHP(0) should floor at one')
        mob:setUnkillable(false)

        mob:setHP(500)
        mob:setAnimation(xi.animation.DEATH)
        assert(mob:restoreHP(50) == 0, 'restoreHP should not restore during death animation')
        assert(mob:getHP() == 500, 'restoreHP during death animation should not change HP')
        mob:setAnimation(xi.animation.NONE)

        assert(npc:getHP() == 0, 'NPC getHP should fall back to zero')
        assert(npc:getHPP() == 0, 'NPC getHPP should fall back to zero')
        assert(npc:getMaxHP() == 0, 'NPC getMaxHP should fall back to zero')
        assert(npc:getBaseHP() == 0, 'NPC getBaseHP should fall back to zero')
        assert(npc:addHP(100) == 0, 'NPC addHP should fall back to zero')
        assert(npc:restoreHP(100) == 0, 'NPC restoreHP should fall back to zero')
        assert(pcall(npc.setHP, npc, 100), 'NPC setHP rejected a valid no-op')
        assert(pcall(npc.setMaxHP, npc, 100), 'NPC setMaxHP rejected a valid no-op')
        assert(pcall(npc.delHP, npc, 100), 'NPC delHP rejected a valid no-op')
        assert(npc:getHP() == 0, 'NPC HP should remain zero after no-op mutators')

        assert(not pcall(mob.getHP), 'getHP accepted missing self')
        assert(not pcall(mob.getHPP), 'getHPP accepted missing self')
        assert(not pcall(mob.getMaxHP), 'getMaxHP accepted missing self')
        assert(not pcall(mob.getBaseHP), 'getBaseHP accepted missing self')
        assert(not pcall(mob.setHP), 'setHP accepted missing self')
        assert(not pcall(mob.setHP, mob), 'setHP accepted missing HP')
        assert(not pcall(mob.setHP, mob, 'bad'), 'setHP accepted non-numeric HP')
        assert(not pcall(mob.setMaxHP), 'setMaxHP accepted missing self')
        assert(not pcall(mob.setMaxHP, mob), 'setMaxHP accepted missing HP')
        assert(not pcall(mob.setMaxHP, mob, 'bad'), 'setMaxHP accepted non-numeric HP')
        assert(not pcall(mob.addHP), 'addHP accepted missing self')
        assert(not pcall(mob.addHP, mob), 'addHP accepted missing HP')
        assert(not pcall(mob.addHP, mob, 'bad'), 'addHP accepted non-numeric HP')
        assert(not pcall(mob.restoreHP), 'restoreHP accepted missing self')
        assert(not pcall(mob.restoreHP, mob), 'restoreHP accepted missing HP')
        assert(not pcall(mob.restoreHP, mob, 'bad'), 'restoreHP accepted non-numeric HP')
        assert(not pcall(mob.delHP), 'delHP accepted missing self')
        assert(not pcall(mob.delHP, mob), 'delHP accepted missing HP')
        assert(not pcall(mob.delHP, mob, 'bad'), 'delHP accepted non-numeric HP')
    end)
end)
