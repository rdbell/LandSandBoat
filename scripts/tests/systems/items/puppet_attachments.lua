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
end)
