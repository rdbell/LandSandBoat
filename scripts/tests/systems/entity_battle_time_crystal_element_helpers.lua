describe('Base entity battle time and crystal element helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('reads battle time and updates mob crystal element while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(type(mob:getBattleTime()) == 'number', 'expected mob battle time to be numeric')
        assert(type(player:getBattleTime()) == 'number', 'expected player battle time to be numeric')
        assert(npc:getBattleTime() == 0, 'expected NPC getBattleTime fallback to be zero')

        assert(type(mob:getCrystalElement()) == 'number', 'expected crystal element to be numeric')
        assert(pcall(mob.setCrystalElement, mob, xi.element.FIRE), 'setCrystalElement rejected a valid mob update')
        assert(mob:getCrystalElement() == xi.element.FIRE, 'expected mob crystal element to update to fire')

        assert(pcall(mob.setCrystalElement, mob, xi.element.DARK), 'setCrystalElement rejected another valid mob update')
        assert(mob:getCrystalElement() == xi.element.DARK, 'expected mob crystal element to update to dark')

        assert(player:getCrystalElement() == xi.element.NONE, 'expected player getCrystalElement fallback to none')
        assert(npc:getCrystalElement() == xi.element.NONE, 'expected NPC getCrystalElement fallback to none')

        assert(pcall(player.setCrystalElement, player, xi.element.ICE), 'player setCrystalElement rejected a valid no-op')
        assert(pcall(npc.setCrystalElement, npc, xi.element.WIND), 'NPC setCrystalElement rejected a valid no-op')
        assert(mob:getCrystalElement() == xi.element.DARK, 'non-mob setCrystalElement should not change mob crystal element')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(type(pet:getBattleTime()) == 'number', 'expected pet battle time to be numeric')
        assert(type(pet:getCrystalElement()) == 'number', 'expected pet crystal element to be numeric')
        assert(pcall(pet.setCrystalElement, pet, xi.element.LIGHT), 'pet setCrystalElement rejected a valid update')
        assert(pet:getCrystalElement() == xi.element.LIGHT, 'expected pet crystal element to update')
        player:despawnPet()

        assert(not pcall(mob.getBattleTime), 'getBattleTime accepted missing self')
        assert(not pcall(mob.getCrystalElement), 'getCrystalElement accepted missing self')
        assert(not pcall(mob.setCrystalElement), 'setCrystalElement accepted missing self')
        assert(not pcall(mob.setCrystalElement, mob), 'setCrystalElement accepted missing element')
        assert(not pcall(mob.setCrystalElement, mob, 'bad'), 'setCrystalElement accepted non-numeric element')
        assert(not pcall(player.setCrystalElement, player, 'bad'), 'non-mob setCrystalElement accepted non-numeric element')
    end)
end)
