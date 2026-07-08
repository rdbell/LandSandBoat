describe('Base entity mob ecology and type helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('reads ecosystem, family, species, and mob type state', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')

        player:addSpell(xi.magic.spell.VALAINERAL)
        player.actions:useSpell(player, xi.magic.spell.VALAINERAL)
        xi.test.world:tickEntity(player)
        xi.test.world:skipTime(10)

        local trust = nil
        for _, member in ipairs(player:getPartyWithTrusts()) do
            if member:getName() == 'valaineral' then
                trust = member
                break
            end
        end

        assert(trust, 'Valaineral was not summoned')

        assert(mob:getEcosystem() == xi.ecosystem.BEAST, 'expected Wild Rabbit ecosystem to be Beast')
        assert(mob:getFamily() == 50, 'expected Wild Rabbit family to be Rabbit')
        assert(mob:getSpecies() == 106, 'expected Wild Rabbit species to be Rabbit')
        assert(mob:isMobType(xi.mobType.NORMAL), 'expected Wild Rabbit to match normal mob type')
        assert(not mob:isMobType(xi.mobType.NOTORIOUS), 'expected Wild Rabbit to not match notorious mob type')
        assert(not mob:isMobType(xi.mobType.BATTLEFIELD), 'expected Wild Rabbit to not match battlefield mob type')
        assert(not mob:isUndead(), 'expected Wild Rabbit to not be undead')
        assert(not mob:isNM(), 'expected Wild Rabbit to not be notorious')

        assert(player:getEcosystem() == xi.ecosystem.HUMANOID, 'expected player ecosystem to be Humanoid')
        assert(player:getFamily() == 0, 'expected player family fallback to be zero')
        assert(player:getSpecies() == 0, 'expected player species fallback to be zero')
        assert(not player:isMobType(xi.mobType.NORMAL), 'expected player to not match normal mob type')
        assert(not player:isUndead(), 'expected player to not be undead')
        assert(not player:isNM(), 'expected player to not be notorious')

        assert(pet:getEcosystem() == xi.ecosystem.ELEMENTAL, 'expected Carbuncle ecosystem to be Elemental')
        assert(pet:getFamily() == 0, 'expected Carbuncle family to fall back to zero')
        assert(pet:getSpecies() == 243, 'expected Carbuncle species to be Carbuncle')
        assert(not pet:isMobType(xi.mobType.NORMAL), 'expected pet to not match normal mob type')
        assert(not pet:isNM(), 'expected pet to not be notorious')

        assert(trust:getEcosystem() == xi.ecosystem.HUMANOID, 'expected Valaineral ecosystem to be Humanoid')
        assert(trust:getFamily() == 0, 'expected Valaineral family to fall back to zero')
        assert(trust:getSpecies() == 293, 'expected Valaineral species to be Elvaan')
        assert(not trust:isMobType(xi.mobType.NORMAL), 'expected trust to not match normal mob type')
        assert(not trust:isNM(), 'expected trust to not be notorious')
        assert(not trust:isUndead(), 'expected trust to not be undead')

        assert(npc:getEcosystem() == 0, 'expected NPC ecosystem fallback to be zero')
        assert(npc:getFamily() == 0, 'expected NPC family fallback to be zero')
        assert(npc:getSpecies() == 0, 'expected NPC species fallback to be zero')
        assert(not npc:isMobType(xi.mobType.NORMAL), 'expected NPC to not match normal mob type')
        assert(not npc:isUndead(), 'expected NPC to not be undead')
        assert(not npc:isNM(), 'expected NPC to not be notorious')

        assert(not pcall(mob.getEcosystem), 'getEcosystem accepted missing self')
        assert(not pcall(mob.getFamily), 'getFamily accepted missing self')
        assert(not pcall(mob.getSpecies), 'getSpecies accepted missing self')
        assert(not pcall(mob.isMobType), 'isMobType accepted missing self')
        assert(not pcall(mob.isMobType, mob), 'isMobType accepted missing type')
        assert(not pcall(mob.isMobType, mob, 'bad'), 'isMobType accepted non-numeric type')
        assert(not pcall(mob.isUndead), 'isUndead accepted missing self')
        assert(not pcall(mob.isNM), 'isNM accepted missing self')
    end)
end)
