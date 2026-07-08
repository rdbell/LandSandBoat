describe('Base entity despoil debuff helper binding', function()
    it('returns despoil effect mappings and zero for unknown items', function()
        local player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
        local npc = player.entities:get('Field_Manual')

        assert(npc, 'Field Manual NPC was not found')

        assert(player:getDespoilDebuff(644) == xi.effect.DEFENSE_DOWN, 'expected Mythril Ore to map to Defense Down')
        assert(player:getDespoilDebuff(842) == xi.effect.ACCURACY_DOWN, 'expected Giant Bird Feather to map to Accuracy Down')
        assert(player:getDespoilDebuff(881) == xi.effect.DEFENSE_DOWN, 'expected Crab Shell to map to Defense Down')
        assert(player:getDespoilDebuff(955) == xi.effect.MAGIC_DEF_DOWN, 'expected Golem Shard to map to Magic Defense Down')
        assert(player:getDespoilDebuff(2334) == xi.effect.MAGIC_ATK_DOWN, 'expected Poroggo Hat to map to Magic Attack Down')
        assert(player:getDespoilDebuff(4376) == xi.effect.ATTACK_DOWN, 'expected Meat Jerky to map to Attack Down')
        assert(player:getDespoilDebuff(4400) == xi.effect.SLOW, 'expected Land Crab Meat to map to Slow')
        assert(player:getDespoilDebuff(1) == 0, 'unknown despoil item should fall back to zero')
        assert(npc:getDespoilDebuff(644) == xi.effect.DEFENSE_DOWN, 'NPC receiver should use the same lookup')

        assert(not pcall(player.getDespoilDebuff), 'getDespoilDebuff accepted missing self')
        assert(not pcall(player.getDespoilDebuff, player), 'getDespoilDebuff accepted missing item ID')
        assert(not pcall(player.getDespoilDebuff, player, 'bad'), 'getDespoilDebuff accepted non-numeric item ID')
        assert(not pcall(npc.getDespoilDebuff, npc, 'bad'), 'NPC getDespoilDebuff accepted non-numeric item ID')
    end)
end)
