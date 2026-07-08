describe('Puppet attachments', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                job   = xi.job.PUP,
                level = 75,
                zone  = xi.zone.SOUTHERN_SAN_DORIA,
            })

        player:setPetName(xi.petType.AUTOMATON, xi.petName.MK_IV)
        player:unlockAttachment(xi.item.HARLEQUIN_FRAME)
        player:unlockAttachment(xi.item.HARLEQUIN_HEAD)
        player:unlockAttachment(xi.item.STROBE_ATTACHMENT)
    end)

    it('equipped attachment is visible on the spawned automaton', function()
        player:setAttachment(xi.item.ATTUNER_ATTACHMENT, 6)
        player:setAttachment(xi.item.STROBE_ATTACHMENT, 5)
        player:spawnPet(xi.petId.AUTOMATON)

        local pet         = player:getPet()
        local attachments = nil
        local item        = nil
        if pet then
            attachments = pet:getAttachments()
        end

        assert(attachments)

        if attachments then
            item = attachments[5]
        end

        assert(item)
        assert(item == 'strobe',
            string.format('expected strobe, got %s', item))
        assert(attachments[0] == '', 'expected empty slot 0')
        assert(attachments[6] == '', 'locked attachment should not equip')
        assert(pet:hasAttachmentSet(xi.item.STROBE_ATTACHMENT), 'expected equipped attachment to be visible')
        assert(not pet:hasAttachmentSet(xi.item.ATTUNER_ATTACHMENT), 'expected absent attachment to be false')
        assert(not player:hasAttachmentSet(xi.item.STROBE_ATTACHMENT), 'non-pet should not have equipped attachments')

        player:setAttachment(xi.item.STROBE_ATTACHMENT, 5)
        assert(pet:hasAttachmentSet(xi.item.STROBE_ATTACHMENT), 'duplicate attachment set should be ignored')

        assert(not pcall(player.hasAttachmentSet), 'hasAttachmentSet accepted missing self')
        assert(not pcall(player.hasAttachmentSet, player), 'hasAttachmentSet accepted missing item')
        assert(not pcall(player.hasAttachmentSet, player, 'bad'), 'hasAttachmentSet accepted non-numeric item')
        assert(not pcall(player.setAttachment), 'setAttachment accepted missing self')
        assert(not pcall(player.setAttachment, player), 'setAttachment accepted missing item')
        assert(not pcall(player.setAttachment, player, xi.item.STROBE_ATTACHMENT), 'setAttachment accepted missing slot')
        assert(not pcall(player.setAttachment, player, xi.item.STROBE_ATTACHMENT, 'bad'), 'setAttachment accepted non-numeric slot')
        assert(not pcall(player.getAttachments), 'getAttachments accepted missing self')
        assert(player:getAttachments() == nil, 'non-automaton getAttachments should return nil')
    end)

    it('tracks unlocked frame, head, and attachment ownership', function()
        assert(player:hasAttachment(xi.item.HARLEQUIN_FRAME), 'expected unlocked frame')
        assert(player:hasAttachment(xi.item.HARLEQUIN_HEAD), 'expected unlocked head')
        assert(player:hasAttachment(xi.item.STROBE_ATTACHMENT), 'expected unlocked attachment')

        assert(not player:hasAttachment(xi.item.ATTUNER_ATTACHMENT), 'expected locked attachment')
        assert(player:unlockAttachment(xi.item.ATTUNER_ATTACHMENT), 'expected fresh unlock to succeed')
        assert(player:hasAttachment(xi.item.ATTUNER_ATTACHMENT), 'expected fresh unlock to be visible')
        assert(not player:unlockAttachment(xi.item.ATTUNER_ATTACHMENT), 'expected duplicate unlock to fail')

        assert(not player:hasAttachment(0), 'expected unknown item to be locked')
        assert(not pcall(player.hasAttachment), 'hasAttachment accepted missing self')
        assert(not pcall(player.hasAttachment, player), 'hasAttachment accepted missing item')
        assert(not pcall(player.hasAttachment, player, 'bad'), 'hasAttachment accepted non-numeric item')
        assert(not pcall(player.unlockAttachment), 'unlockAttachment accepted missing self')
        assert(not pcall(player.unlockAttachment, player), 'unlockAttachment accepted missing item')
        assert(not pcall(player.unlockAttachment, player, 'bad'), 'unlockAttachment accepted non-numeric item')
    end)

    it('reads the configured automaton name', function()
        assert(player:getAutomatonName() == 'Mk.IV', 'expected configured automaton name')
        assert(not pcall(player.getAutomatonName), 'getAutomatonName accepted missing self')
        player:spawnPet(xi.petId.AUTOMATON)
        local pet = player:getPet()
        assert(pet, 'Automaton was not summoned')
        assert(pet:getAutomatonName() == '', 'non-player should return empty name')
    end)

    it('counts and removes active maneuvers', function()
        player:addStatusEffect(xi.effect.PROTECT, { power = 1, duration = 60, origin = player })
        player:addStatusEffect(xi.effect.FIRE_MANEUVER, { power = 1, duration = 60, origin = player })
        xi.test.world:skipTime(1)
        player:addStatusEffect(xi.effect.ICE_MANEUVER, { power = 1, duration = 60, origin = player })
        xi.test.world:skipTime(1)
        player:addStatusEffect(xi.effect.LIGHT_MANEUVER, { power = 1, duration = 60, origin = player })

        assert(player:getActiveManeuverCount() == 3, 'expected three active maneuvers')
        player:removeOldestManeuver()
        assert(player:getActiveManeuverCount() == 2, 'expected one maneuver to be removed')
        assert(not player:hasStatusEffect(xi.effect.FIRE_MANEUVER), 'oldest maneuver should be removed')
        assert(player:hasStatusEffect(xi.effect.ICE_MANEUVER), 'newer maneuver should remain')

        player:removeAllManeuvers()
        assert(player:getActiveManeuverCount() == 0, 'expected all maneuvers to be removed')
        assert(player:hasStatusEffect(xi.effect.PROTECT), 'non-maneuver effect should remain')

        assert(not pcall(player.getActiveManeuverCount), 'getActiveManeuverCount accepted missing self')
        assert(not pcall(player.removeOldestManeuver), 'removeOldestManeuver accepted missing self')
        assert(not pcall(player.removeAllManeuvers), 'removeAllManeuvers accepted missing self')
    end)

    it('reads and updates automaton frame and head with ownership gates', function()
        assert(player:getAutomatonFrame() == xi.automaton.frame.HARLEQUIN, 'expected default Harlequin frame')
        assert(player:getAutomatonHead() == xi.automaton.head.HARLEQUIN, 'expected default Harlequin head')

        player:setAutomatonFrame(xi.automaton.frame.VALOREDGE)
        assert(player:getAutomatonFrame() == xi.automaton.frame.HARLEQUIN, 'locked frame should not equip')

        player:unlockAttachment(xi.item.VALOREDGE_FRAME)
        player:setAutomatonFrame(xi.automaton.frame.VALOREDGE)
        assert(player:getAutomatonFrame() == xi.automaton.frame.VALOREDGE, 'unlocked frame should equip')

        player:setAutomatonHead(xi.automaton.head.SOULSOOTHER)
        assert(player:getAutomatonHead() == xi.automaton.head.HARLEQUIN, 'locked head should not equip')

        player:unlockAttachment(xi.item.SOULSOOTHER_HEAD)
        player:setAutomatonHead(xi.automaton.head.SOULSOOTHER)
        assert(player:getAutomatonHead() == xi.automaton.head.SOULSOOTHER, 'unlocked head should equip')

        player:spawnPet(xi.petId.AUTOMATON)
        local pet = player:getPet()
        assert(pet, 'Automaton was not summoned')
        assert(pet:getAutomatonFrame() == xi.automaton.frame.VALOREDGE, 'pet should expose equipped frame')
        assert(pet:getAutomatonHead() == xi.automaton.head.SOULSOOTHER, 'pet should expose equipped head')

        assert(not pcall(player.getAutomatonFrame), 'getAutomatonFrame accepted missing self')
        assert(not pcall(player.getAutomatonHead), 'getAutomatonHead accepted missing self')
        assert(not pcall(player.setAutomatonFrame), 'setAutomatonFrame accepted missing self')
        assert(not pcall(player.setAutomatonFrame, player), 'setAutomatonFrame accepted missing frame')
        assert(not pcall(player.setAutomatonFrame, player, 'bad'), 'setAutomatonFrame accepted non-numeric frame')
        assert(not pcall(player.setAutomatonHead), 'setAutomatonHead accepted missing self')
        assert(not pcall(player.setAutomatonHead, player), 'setAutomatonHead accepted missing head')
        assert(not pcall(player.setAutomatonHead, player, 'bad'), 'setAutomatonHead accepted non-numeric head')
    end)
end)
