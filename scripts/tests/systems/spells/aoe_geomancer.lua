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

    describe('Theurgic Focus #geo', function()
        before_each(function()
            p1:changeJob(xi.job.GEO)
            p1:setLevel(99)
        end)

        it('halves the radius of -ra spells (Fira to Watera)', function()
            p1:addStatusEffect(xi.effect.THEURGIC_FOCUS, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.FIRA)
            assert(spell, 'FIRA spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[1] == xi.magic.aoe.RADIAL, 'Expected RADIAL type')
            assert(result[2] == 5, 'Expected radius 5y')
        end)

        it('does not affect non -ra spells', function()
            p1:addStatusEffect(xi.effect.THEURGIC_FOCUS, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.FIRE)
            assert(spell, 'FIRE spell not found')
            local baseRadius = spell:getRadius()
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[2] == baseRadius, 'Radius should remain unchanged')
        end)
    end)

end)
