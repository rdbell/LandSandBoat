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

    describe('Songs #brd', function()
        before_each(function()
            p1:changeJob(xi.job.BRD)
            p1:setLevel(99)
            p1:addItem(xi.item.GJALLARHORN_99)
            p1:addSpell(xi.magic.spell.MAGES_BALLAD)
        end)

        it('forces single target with Pianissimo effect and removes it after use', function()
            p1.actions:useAbility(p1, xi.jobAbility.PIANISSIMO)
            xi.test.world:tickEntity(p1)
            p1.assert:hasEffect(xi.effect.PIANISSIMO)
            p1.actions:useSpell(p2, xi.magic.spell.MAGES_BALLAD)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p2.assert:hasEffect(xi.effect.BALLAD)
            p1.assert.no:hasEffect(xi.effect.BALLAD)
            p1.assert.no:hasEffect(xi.effect.PIANISSIMO)
        end)

        it('returns base radius without String instrument equipped', function()
            p1:setSkillLevel(xi.skill.STRING_INSTRUMENT, 424)
            local s = spy('xi.combat.magicAoE.calculateTypeAndRadius')
            p1.actions:useSpell(p1, xi.magic.spell.MAGES_BALLAD)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(10)
            s:called(1)
            assert(s.calls[1].returned[1] == xi.magic.aoe.RADIAL, 'Expected RADIAL aoe type')
            assert(s.calls[1].returned[2] == 10, 'Expected base radius of 10')
        end)

        it('scales radius from 1.0x to 2.0x based on String skill vs song level cap', function()
            p1:addItem(xi.item.MAPLE_HARP)
            p1:equipItem(xi.item.MAPLE_HARP, nil, xi.slot.RANGED)
            local spell = GetSpell(xi.magic.spell.MAGES_BALLAD)
            assert(spell)
            local testCases =
            {
                { skill = 0,   expectedRadius = 10 },
                { skill = 79,  expectedRadius = 10 },
                { skill = 80,  expectedRadius = 11 },
                { skill = 86,  expectedRadius = 11 },
                { skill = 87,  expectedRadius = 12 },
                { skill = 93,  expectedRadius = 12 },
                { skill = 94,  expectedRadius = 13 },
                { skill = 100, expectedRadius = 13 },
                { skill = 101, expectedRadius = 14 },
                { skill = 107, expectedRadius = 14 },
                { skill = 108, expectedRadius = 15 },
                { skill = 115, expectedRadius = 15 },
                { skill = 116, expectedRadius = 16 },
                { skill = 122, expectedRadius = 16 },
                { skill = 123, expectedRadius = 17 },
                { skill = 129, expectedRadius = 17 },
                { skill = 130, expectedRadius = 18 },
                { skill = 136, expectedRadius = 18 },
                { skill = 137, expectedRadius = 19 },
                { skill = 143, expectedRadius = 19 },
                { skill = 144, expectedRadius = 20 },
                { skill = 424, expectedRadius = 20 },
            }

            for _, tc in ipairs(testCases) do
                p1:setSkillLevel(xi.skill.STRING_INSTRUMENT, tc.skill * 10)
                local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
                assert(result[1] == xi.magic.aoe.RADIAL,
                    string.format('Skill %d: Expected RADIAL type', tc.skill))
                assert(result[2] == tc.expectedRadius,
                    string.format('Skill %d: Expected radius %d, got %s', tc.skill, tc.expectedRadius, tostring(result[2])))
            end
        end)
    end)
end)
