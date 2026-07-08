describe('Base entity addHPLeaveSleeping binding', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('restores battle entity HP without removing sleep effects', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')
        mob:spawn()

        mob:setMaxHP(1000)
        mob:setHP(333)
        assert(mob:addHPLeaveSleeping(200) == 200, 'addHPLeaveSleeping did not report restored HP')
        assert(mob:getHP() == 533, 'addHPLeaveSleeping did not restore HP')

        assert(mob:addHPLeaveSleeping(9999) == 467, 'addHPLeaveSleeping did not report capped restore')
        assert(mob:getHP() == 1000, 'addHPLeaveSleeping did not cap HP')

        mob:setHP(0)
        assert(mob:addHPLeaveSleeping(40) == 40, 'addHPLeaveSleeping did not restore from zero HP')
        assert(mob:getHP() == 40, 'addHPLeaveSleeping did not update zero HP')

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
            assert(mob:addHPLeaveSleeping(1) == 1, 'addHPLeaveSleeping did not report one restored HP')
            assert(mob:hasStatusEffect(effect), string.format('addHPLeaveSleeping should preserve effect %u', effect))
            mob:delStatusEffect(effect)
        end

        mob:setHP(500)
        mob:setUnkillable(true)
        mob:addHPLeaveSleeping(-999)
        assert(mob:getHP() == 1, 'unkillable mob addHPLeaveSleeping damage should floor at one')
        mob:setUnkillable(false)

        player:setMaxHP(1000)
        player:setHP(500)
        player:setAnimation(xi.animation.SIT)
        assert(player:addHPLeaveSleeping(-50) == 50, 'sitting player damage should report HP loss')
        assert(player:getHP() == 450, 'sitting player damage did not subtract HP')
        assert(player:getAnimation() == xi.animation.NONE, 'sitting player damage should clear sit animation')

        player:setHP(500)
        player:addStatusEffect(xi.effect.HEALING, { power = 1, duration = 60, origin = player })
        assert(player:hasStatusEffect(xi.effect.HEALING), 'healing effect was not applied')
        assert(player:getAnimation() == xi.animation.HEALING, 'healing effect did not set healing animation')
        assert(player:addHPLeaveSleeping(-50) == 50, 'healing player damage should report HP loss')
        assert(player:getHP() == 450, 'healing player damage did not subtract HP')
        assert(not player:hasStatusEffect(xi.effect.HEALING), 'healing player damage should remove healing effect')
        assert(player:getAnimation() == xi.animation.NONE, 'healing player damage should clear healing animation')

        player:setHP(500)
        player:setAnimation(xi.animation.HEALING)
        assert(not player:hasStatusEffect(xi.effect.HEALING), 'player should not have bare healing effect')
        assert(player:addHPLeaveSleeping(-50) == 50, 'bare healing animation damage should report HP loss')
        assert(player:getHP() == 450, 'bare healing animation damage did not subtract HP')
        assert(player:getAnimation() == xi.animation.HEALING, 'bare healing animation should remain without healing effect')
        player:setAnimation(xi.animation.NONE)

        player:changeJob(xi.job.PUP)
        player:setLevel(75)
        player:setPetName(xi.petType.AUTOMATON, xi.petName.MK_IV)
        player:unlockAttachment(xi.item.HARLEQUIN_FRAME)
        player:unlockAttachment(xi.item.HARLEQUIN_HEAD)
        player:spawnPet(xi.petId.AUTOMATON)
        local automaton = player:getPet()
        assert(automaton, 'Automaton was not summoned')
        player:setHP(500)
        player:setAnimation(xi.animation.SIT)
        automaton:setAnimation(xi.animation.SIT)
        assert(player:addHPLeaveSleeping(-50) == 50, 'sitting player with automaton damage should report HP loss')
        assert(player:getAnimation() == xi.animation.NONE, 'sitting player with automaton should stand up')
        assert(automaton:getAnimation() == xi.animation.NONE, 'sitting automaton should stand up with player')
        player:despawnPet()

        assert(npc:addHPLeaveSleeping(100) == 0, 'NPC addHPLeaveSleeping should fall back to zero')
        assert(npc:getHP() == 0, 'NPC HP should remain zero after addHPLeaveSleeping')

        assert(not pcall(mob.addHPLeaveSleeping), 'addHPLeaveSleeping accepted missing self')
        assert(not pcall(mob.addHPLeaveSleeping, mob), 'addHPLeaveSleeping accepted missing HP')
        assert(not pcall(mob.addHPLeaveSleeping, mob, 'bad'), 'addHPLeaveSleeping accepted non-numeric HP')
    end)
end)
