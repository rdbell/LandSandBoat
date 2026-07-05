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

    describe('Majesty #pld', function()
        before_each(function()
            p1:changeJob(xi.job.PLD)
            p1:setLevel(99)
            p1:addSpell(xi.magic.spell.CURE)
            p1:addSpell(xi.magic.spell.PROTECT)
            p1:addSpell(xi.magic.spell.SHELL)
            p1.actions:useAbility(p1, xi.jobAbility.MAJESTY)
            xi.test.world:tickEntity(p1)
            p1.assert:hasEffect(xi.effect.MAJESTY)
        end)

        it('converts Cure spells to 10y AoE', function()
            p1:setHP(1)
            p2:setHP(1)
            p1.actions:useSpell(p1, xi.magic.spell.CURE)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            assert(p1:getHP() > 1, 'p1 HP should be greater than 1')
            assert(p2:getHP() > 1, 'p2 HP should be greater than 1')
        end)

        it('converts Protect spells to 10y AoE', function()
            p1.actions:useSpell(p1, xi.magic.spell.PROTECT)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert:hasEffect(xi.effect.PROTECT)
            p2.assert:hasEffect(xi.effect.PROTECT)
        end)

        it('does not affect non-Cure/Protect spells', function()
            p1.actions:useSpell(p1, xi.magic.spell.SHELL)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert:hasEffect(xi.effect.SHELL)
            p2.assert.no:hasEffect(xi.effect.SHELL)
        end)
    end)

end)
