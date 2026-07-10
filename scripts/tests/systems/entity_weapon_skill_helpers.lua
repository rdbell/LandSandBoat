describe('Base entity weapon skill type and level helpers', function()
    it('reads equipped weapon skill metadata and PC weapon skill levels', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        -- Empty slots return zero for skill type/level/subskill.
        assert(player:getWeaponSkillType(xi.slot.MAIN) == 0 or type(player:getWeaponSkillType(xi.slot.MAIN)) == 'number',
            'getWeaponSkillType should return a number')
        local mainSkillType = player:getWeaponSkillType(xi.slot.MAIN)
        local mainSkillLevel = player:getWeaponSkillLevel(xi.slot.MAIN)
        assert(type(mainSkillLevel) == 'number', 'getWeaponSkillLevel should return a number')

        -- When a weapon is equipped, skill level matches getSkillLevel of that skill type.
        if mainSkillType ~= 0 then
            assert(mainSkillLevel == player:getSkillLevel(mainSkillType),
                string.format('weapon skill level %u should match skill %u level %u',
                    mainSkillLevel, mainSkillType, player:getSkillLevel(mainSkillType)))
        end

        -- Out-of-range slots fall back to zero.
        assert(player:getWeaponSkillLevel(4) == 0, 'getWeaponSkillLevel(4) should be zero')
        assert(player:getWeaponSkillType(4) == 0, 'getWeaponSkillType(4) should be zero')
        assert(player:getWeaponDamageType(4) == 0, 'getWeaponDamageType(4) should be zero')
        assert(player:getWeaponSubSkillType(4) == 0, 'getWeaponSubSkillType(4) should be zero')

        -- Mobs expose skill/damage type from their weapons; skill level is PC-only.
        local mobSkillType = mob:getWeaponSkillType(xi.slot.MAIN)
        assert(type(mobSkillType) == 'number', 'mob getWeaponSkillType should return a number')
        assert(mob:getWeaponSkillLevel(xi.slot.MAIN) == 0, 'mob getWeaponSkillLevel should be zero')
        assert(mob:getWeaponSubSkillType(xi.slot.MAIN) == 0, 'mob getWeaponSubSkillType should be zero')

        -- NPCs fall back to zero for all four helpers.
        assert(npc:getWeaponSkillType(xi.slot.MAIN) == 0, 'NPC getWeaponSkillType should be zero')
        assert(npc:getWeaponDamageType(xi.slot.MAIN) == 0, 'NPC getWeaponDamageType should be zero')
        assert(npc:getWeaponSkillLevel(xi.slot.MAIN) == 0, 'NPC getWeaponSkillLevel should be zero')
        assert(npc:getWeaponSubSkillType(xi.slot.MAIN) == 0, 'NPC getWeaponSubSkillType should be zero')

        assert(not pcall(player.getWeaponSkillLevel), 'getWeaponSkillLevel accepted missing self')
        assert(not pcall(player.getWeaponSkillLevel, player), 'getWeaponSkillLevel accepted missing slot')
        assert(not pcall(player.getWeaponSkillLevel, player, 'bad'), 'getWeaponSkillLevel accepted non-numeric slot')
        assert(not pcall(player.getWeaponSkillType), 'getWeaponSkillType accepted missing self')
        assert(not pcall(player.getWeaponSkillType, player), 'getWeaponSkillType accepted missing slot')
        assert(not pcall(player.getWeaponDamageType, player, 'bad'), 'getWeaponDamageType accepted non-numeric slot')
        assert(not pcall(player.getWeaponSubSkillType, player, 'bad'), 'getWeaponSubSkillType accepted non-numeric slot')
    end)
end)
