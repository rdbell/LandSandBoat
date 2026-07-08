describe('Base entity mob combat control helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
    end)

    it('updates mob controller gates and mob skill attack list while invalid receivers fall back', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')

        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        assert(mob:isRangedAttackEnabled() == false, 'expected ranged attack to default to false')

        assert(pcall(mob.setAutoAttackEnabled, mob, false), 'setAutoAttackEnabled rejected a valid mob update')
        assert(pcall(mob.setRangedAttackEnabled, mob, true), 'setRangedAttackEnabled rejected a valid mob update')
        assert(pcall(mob.setMagicCastingEnabled, mob, false), 'setMagicCastingEnabled rejected a valid mob update')
        assert(pcall(mob.setMobAbilityEnabled, mob, false), 'setMobAbilityEnabled rejected a valid mob update')
        assert(pcall(mob.setMobSkillAttack, mob, 732), 'setMobSkillAttack rejected a valid mob update')
        assert(mob:isRangedAttackEnabled() == true, 'expected ranged attack to update')
        assert(mob:getMobMod(xi.mobMod.ATTACK_SKILL_LIST) == 732, 'expected attack skill list mob mod to update')

        assert(pcall(mob.setRangedAttackEnabled, mob, false), 'setRangedAttackEnabled rejected a valid clear')
        assert(mob:isRangedAttackEnabled() == false, 'expected ranged attack to clear')

        assert(player:isRangedAttackEnabled() == false, 'expected player isRangedAttackEnabled fallback to false')
        assert(npc:isRangedAttackEnabled() == false, 'expected NPC isRangedAttackEnabled fallback to false')

        assert(pcall(player.setAutoAttackEnabled, player, false), 'player setAutoAttackEnabled rejected a valid no-op')
        assert(pcall(player.setRangedAttackEnabled, player, true), 'player setRangedAttackEnabled rejected a valid no-op')
        assert(pcall(player.setMagicCastingEnabled, player, false), 'player setMagicCastingEnabled rejected a valid no-op')
        assert(pcall(player.setMobAbilityEnabled, player, false), 'player setMobAbilityEnabled rejected a valid no-op')
        assert(pcall(player.setMobSkillAttack, player, 111), 'player setMobSkillAttack rejected a valid no-op')
        assert(pcall(npc.setAutoAttackEnabled, npc, false), 'NPC setAutoAttackEnabled rejected a valid no-op')
        assert(pcall(npc.setRangedAttackEnabled, npc, true), 'NPC setRangedAttackEnabled rejected a valid no-op')
        assert(pcall(npc.setMagicCastingEnabled, npc, false), 'NPC setMagicCastingEnabled rejected a valid no-op')
        assert(pcall(npc.setMobAbilityEnabled, npc, false), 'NPC setMobAbilityEnabled rejected a valid no-op')
        assert(pcall(npc.setMobSkillAttack, npc, 222), 'NPC setMobSkillAttack rejected a valid no-op')
        assert(player:getMobMod(xi.mobMod.ATTACK_SKILL_LIST) == 0, 'player setMobSkillAttack should not update mob mods')
        assert(npc:getMobMod(xi.mobMod.ATTACK_SKILL_LIST) == 0, 'NPC setMobSkillAttack should not update mob mods')
        assert(mob:getMobMod(xi.mobMod.ATTACK_SKILL_LIST) == 732, 'invalid receiver calls should not change mob state')

        player:spawnPet(xi.petId.CARBUNCLE)
        local pet = player:getPet()
        assert(pet, 'Carbuncle was not summoned')
        assert(pcall(pet.setAutoAttackEnabled, pet, false), 'pet setAutoAttackEnabled rejected a valid update')
        assert(pcall(pet.setRangedAttackEnabled, pet, true), 'pet setRangedAttackEnabled rejected a valid update')
        assert(pcall(pet.setMagicCastingEnabled, pet, false), 'pet setMagicCastingEnabled rejected a valid update')
        assert(pcall(pet.setMobAbilityEnabled, pet, false), 'pet setMobAbilityEnabled rejected a valid update')
        assert(pcall(pet.setMobSkillAttack, pet, 39), 'pet setMobSkillAttack rejected a valid update')
        assert(pet:isRangedAttackEnabled() == true, 'expected pet ranged attack to update')
        assert(pet:getMobMod(xi.mobMod.ATTACK_SKILL_LIST) == 39, 'expected pet attack skill list mob mod to update')
        player:despawnPet()

        assert(not pcall(mob.setAutoAttackEnabled), 'setAutoAttackEnabled accepted missing self')
        assert(not pcall(mob.setAutoAttackEnabled, mob), 'setAutoAttackEnabled accepted missing state')
        assert(not pcall(mob.setAutoAttackEnabled, mob, 'bad'), 'setAutoAttackEnabled accepted non-boolean state')
        assert(not pcall(player.setAutoAttackEnabled, player, 'bad'), 'non-mob setAutoAttackEnabled accepted non-boolean state')

        assert(not pcall(mob.setRangedAttackEnabled), 'setRangedAttackEnabled accepted missing self')
        assert(not pcall(mob.setRangedAttackEnabled, mob), 'setRangedAttackEnabled accepted missing state')
        assert(not pcall(mob.setRangedAttackEnabled, mob, 'bad'), 'setRangedAttackEnabled accepted non-boolean state')
        assert(not pcall(mob.isRangedAttackEnabled), 'isRangedAttackEnabled accepted missing self')

        assert(not pcall(mob.setMagicCastingEnabled), 'setMagicCastingEnabled accepted missing self')
        assert(not pcall(mob.setMagicCastingEnabled, mob), 'setMagicCastingEnabled accepted missing state')
        assert(not pcall(mob.setMagicCastingEnabled, mob, 'bad'), 'setMagicCastingEnabled accepted non-boolean state')

        assert(not pcall(mob.setMobAbilityEnabled), 'setMobAbilityEnabled accepted missing self')
        assert(not pcall(mob.setMobAbilityEnabled, mob), 'setMobAbilityEnabled accepted missing state')
        assert(not pcall(mob.setMobAbilityEnabled, mob, 'bad'), 'setMobAbilityEnabled accepted non-boolean state')

        assert(not pcall(mob.setMobSkillAttack), 'setMobSkillAttack accepted missing self')
        assert(not pcall(mob.setMobSkillAttack, mob), 'setMobSkillAttack accepted missing list ID')
        assert(not pcall(mob.setMobSkillAttack, mob, 'bad'), 'setMobSkillAttack accepted non-numeric list ID')
        assert(not pcall(player.setMobSkillAttack, player, 'bad'), 'non-mob setMobSkillAttack accepted non-numeric list ID')
    end)
end)
