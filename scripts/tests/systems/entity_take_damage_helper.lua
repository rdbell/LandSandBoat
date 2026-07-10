describe('Base entity takeDamage helper binding', function()
    it('applies damage and honors wake-up and petrification flags', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        mob:setMaxHP(1000)
        mob:setHP(1000)
        mob:setMaxMP(200)
        mob:setMP(0)
        mob:addMod(xi.mod.ABSORB_DMG_TO_MP, 25)
        mob:addStatusEffect(xi.effect.SLEEP_I, { origin = mob, tier = 1, duration = 60 })
        mob:takeDamage(200, player, 4, 7)
        assert(mob:getHP() == 800, 'takeDamage did not apply HP loss')
        assert(mob:getMP() == 50, 'takeDamage did not absorb damage into MP')
        assert(not mob:hasStatusEffect(xi.effect.SLEEP_I), 'default damage should wake sleep')

        mob:addStatusEffect(xi.effect.SLEEP_I, { origin = mob, tier = 1, duration = 60 })
        mob:takeDamage(10, nil, nil, nil, { wakeUp = false, breakBind = true })
        assert(mob:hasStatusEffect(xi.effect.SLEEP_I), 'wakeUp=false should preserve sleep')
        mob:takeDamage(10, nil, nil, nil, {})
        assert(mob:hasStatusEffect(xi.effect.SLEEP_I), 'an empty flags table should preserve sleep')
        mob:delStatusEffect(xi.effect.SLEEP_I)

        mob:addStatusEffect(xi.effect.SLEEP_I, { origin = mob, tier = 1, duration = 60 })
        mob:takeDamage(1, nil, nil, nil, { wakeUp = true, breakBind = false })
        assert(not mob:hasStatusEffect(xi.effect.SLEEP_I), 'ordinary sleep should wake when only breakBind is false')

        mob:addStatusEffect(xi.effect.SLEEP_II, { origin = mob, duration = 60 })
        mob:takeDamage(1)
        assert(not mob:hasStatusEffect(xi.effect.SLEEP_II), 'damage should wake Sleep II')
        mob:addStatusEffect(xi.effect.LULLABY, { origin = mob, duration = 60 })
        mob:takeDamage(1)
        assert(not mob:hasStatusEffect(xi.effect.LULLABY), 'damage should wake Lullaby')

        mob:addStatusEffect(xi.effect.SLEEP_I, { origin = mob, tier = 4, duration = 60 })
        mob:takeDamage(1, nil, nil, nil, { wakeUp = true, breakBind = false })
        assert(mob:hasStatusEffect(xi.effect.SLEEP_I), 'Nightmare with breakBind=false should preserve sleep')
        mob:delStatusEffect(xi.effect.SLEEP_I)

        mob:addStatusEffect(xi.effect.PETRIFICATION, { origin = mob, subPower = 1, duration = 60 })
        mob:takeDamage(10)
        assert(not mob:hasStatusEffect(xi.effect.PETRIFICATION), 'subPower=1 petrification should be removed')

        mob:addStatusEffect(xi.effect.PETRIFICATION, { origin = mob, subPower = 0, duration = 60 })
        mob:takeDamage(10, nil, nil, nil, { wakeUp = false, breakBind = true, removePetrify = true })
        assert(not mob:hasStatusEffect(xi.effect.PETRIFICATION), 'removePetrify should remove petrification')

        mob:addStatusEffect(xi.effect.SLEEP_I, { origin = mob, tier = 11, duration = 60 })
        mob:takeDamage(10)
        assert(mob:hasStatusEffect(xi.effect.SLEEP_I), 'Diabolos Nightmare should preserve sleep')
        mob:delStatusEffect(xi.effect.SLEEP_I)

        mob:setHP(500)
        mob:takeDamage(100, npc)
        assert(mob:getHP() == 500, 'NPC attackers should be rejected')

        assert(pcall(npc.takeDamage, npc, 100, player), 'NPC takeDamage should be a safe no-op')
        assert(npc:getHP() == 0, 'NPC takeDamage should not change HP')

        assert(not pcall(mob.takeDamage), 'takeDamage accepted missing self')
        assert(not pcall(mob.takeDamage, mob, 'bad'), 'takeDamage accepted non-numeric damage')
        assert(not pcall(mob.takeDamage, mob, 1, 'bad'), 'takeDamage accepted invalid attacker')
        assert(not pcall(mob.takeDamage, mob, 1, nil, 'bad'), 'takeDamage accepted non-numeric attack type')
        assert(not pcall(mob.takeDamage, mob, 1, nil, nil, 'bad'), 'takeDamage accepted non-numeric damage type')
        assert(not pcall(mob.takeDamage, mob, 1, nil, nil, nil, 'bad'), 'takeDamage accepted non-table flags')
        assert(not pcall(mob.takeDamage, mob, 1, nil, nil, nil, { wakeUp = 'bad' }), 'takeDamage accepted non-boolean flags')
    end)
end)
