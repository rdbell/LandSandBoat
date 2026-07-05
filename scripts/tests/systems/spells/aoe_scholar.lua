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

    describe('Accession #sch', function()
        before_each(function()
            p1:changeJob(xi.job.SCH)
            p1:setLevel(99)
            p1:changesJob(xi.job.RDM)
            p1:setsLevel(49)
            p1:addSpell(xi.magic.spell.PROTECT)
            p1:addSpell(xi.magic.spell.HASTE)
            p1.actions:useAbility(p1, xi.jobAbility.LIGHT_ARTS)
            xi.test.world:tickEntity(p1)
            p1.actions:useAbility(p1, xi.jobAbility.ACCESSION)
            xi.test.world:tickEntity(p1)
            p1.assert:hasEffect(xi.effect.ACCESSION)
        end)

        it('converts RADIAL_ACCE spells to 10y AoE', function()
            p1.actions:useSpell(p1, xi.magic.spell.PROTECT)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert:hasEffect(xi.effect.PROTECT)
            p2.assert:hasEffect(xi.effect.PROTECT)
        end)

        it('does not affect spells without RADIAL_ACCE type', function()
            p1.actions:useSpell(p1, xi.magic.spell.HASTE)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert:hasEffect(xi.effect.HASTE)
            p2.assert.no:hasEffect(xi.effect.HASTE)
        end)
    end)

    describe('Manifestation #sch', function()
        before_each(function()
            p1:changeJob(xi.job.SCH)
            p1:setLevel(99)
        end)

        it('converts RADIAL_MANI spells to 10y AoE', function()
            p1:addStatusEffect(xi.effect.MANIFESTATION, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.SLEEP)
            assert(spell, 'SLEEP spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[1] == xi.magic.aoe.RADIAL, 'Expected RADIAL type')
            assert(result[2] == 10, 'Expected radius 10')
        end)

        it('does not affect spells without RADIAL_MANI type', function()
            p1:addStatusEffect(xi.effect.MANIFESTATION, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.STONE)
            assert(spell, 'STONE spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[1] ~= xi.magic.aoe.RADIAL or result[2] ~= 10,
                'Should not convert to 10y AoE')
        end)
    end)

end)
