describe('AoE', function()
    ---@type CClientEntityPair
    local p1
    ---@type CClientEntityPair
    local p2

    before_each(function()
        p1 = xi.test.world:spawnPlayer(
            {
                zone  = xi.zone.GM_HOME,
                job   = xi.job.BLM,
                level = 99,
            })
        p2 = xi.test.world:spawnPlayer(
            {
                zone  = xi.zone.GM_HOME,
                job   = xi.job.BLM,
                level = 99,
            })
        p1.actions:inviteToParty(p2)
        p2.actions:acceptPartyInvite()
    end)

    describe('Divine Veil #whm', function()
        before_each(function()
            p1:changeJob(xi.job.WHM)
            p1:setLevel(99)
            p1:addSpell(xi.magic.spell.ERASE)
            p1:addSpell(xi.magic.spell.PARALYNA)
            p1:addSpell(xi.magic.spell.PROTECT)
        end)

        it('converts -na spells to 10y AoE with Divine Seal', function()
            p2:addStatusEffect(xi.effect.PARALYSIS, { power = 1, duration = 60, origin = p2 })
            p2.assert:hasEffect(xi.effect.PARALYSIS)
            p1.actions:useAbility(p1, xi.jobAbility.DIVINE_SEAL)
            xi.test.world:tickEntity(p1)
            p1.actions:useSpell(p1, xi.magic.spell.PARALYNA)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p2.assert.no:hasEffect(xi.effect.PARALYSIS)
        end)

        it('converts -na spells to 10y AoE with AOE_NA mod chance', function()
            p2:addStatusEffect(xi.effect.PARALYSIS, { power = 1, duration = 60, origin = p2 })
            p2.assert:hasEffect(xi.effect.PARALYSIS)
            p1:addItem(xi.item.YAGRUSH_75)
            p1:equipItem(xi.item.YAGRUSH_75, nil, xi.slot.MAIN)
            p1.actions:useSpell(p1, xi.magic.spell.PARALYNA)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p2.assert.no:hasEffect(xi.effect.PARALYSIS)
        end)

        it('converts Erase to 10y AoE with Divine Seal', function()
            p1:addStatusEffect(xi.effect.SLOW, { power = 1, duration = 60, origin = p1 })
            p2:addStatusEffect(xi.effect.SLOW, { power = 1, duration = 60, origin = p2 })
            p1.assert:hasEffect(xi.effect.SLOW)
            p2.assert:hasEffect(xi.effect.SLOW)
            p1.actions:useAbility(p1, xi.jobAbility.DIVINE_SEAL)
            xi.test.world:tickEntity(p1)
            p1.actions:useSpell(p1, xi.magic.spell.ERASE)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert.no:hasEffect(xi.effect.SLOW)
            p2.assert.no:hasEffect(xi.effect.SLOW)
        end)

        it('does not convert without Divine Veil trait', function()
            p1:setLevel(49)
            p2:addStatusEffect(xi.effect.PARALYSIS, { power = 1, duration = 60, origin = p2 })
            p2.assert:hasEffect(xi.effect.PARALYSIS)
            p1.actions:useAbility(p1, xi.jobAbility.DIVINE_SEAL)
            xi.test.world:tickEntity(p1)
            p1.actions:useSpell(p1, xi.magic.spell.PARALYNA)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p2.assert:hasEffect(xi.effect.PARALYSIS)
        end)

        it('does not convert without Divine Seal or AOE_NA mod', function()
            p2:addStatusEffect(xi.effect.PARALYSIS, { power = 1, duration = 60, origin = p2 })
            p2.assert:hasEffect(xi.effect.PARALYSIS)
            p1.actions:useSpell(p1, xi.magic.spell.PARALYNA)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p2.assert:hasEffect(xi.effect.PARALYSIS)
        end)
    end)

end)
