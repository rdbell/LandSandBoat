require('scripts/globals/campaign')

describe('Campaign Sigil application finish', function()
    it('replaces influence effects, applies the chosen Sigil mask, and charges paid bonuses', function()
        local originalGetMedalRank = xi.campaign.getMedalRank
        local calls = {}
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            delStatusEffectsByFlag = function(_, flag, force)
                calls.removedFlag = flag
                calls.removedForce = force
            end,
            addStatusEffect = function(_, effect, args)
                calls.effect = effect
                calls.effectArgs = args
            end,
            messageSpecial = function(_, message) calls.message = message end,
            delCurrency = function(_, currency, amount)
                calls.currency = currency
                calls.amount = amount
            end,
        }

        xi.campaign.getMedalRank = function() return 2 end
        -- Mask 0x0F: Regen (free) plus Refresh, meal-duration, and EXP-loss
        -- reduction (three paid +50 effects).
        xi.campaign.sigilOnEventFinish(player, 13, bit.bor(bit.lshift(0x0F, 11), 1))

        assert(calls.removedFlag == xi.effectFlag.INFLUENCE and calls.removedForce)
        assert(calls.effect == xi.effect.SIGIL)
        assert(calls.effectArgs.power == 0x0F and calls.effectArgs.duration == 12600)
        assert(calls.effectArgs.origin == player and calls.effectArgs.subPower == 35)
        assert(calls.message == zones[xi.zone.BASTOK_MARKETS_S].text.ALLIED_SIGIL)
        assert(calls.currency == 'allied_notes' and calls.amount == 150)

        xi.campaign.getMedalRank = originalGetMedalRank
    end)

    it('does nothing for a cancelled Sigil event', function()
        local calls = 0
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            delStatusEffectsByFlag = function() calls = calls + 1 end,
            addStatusEffect = function() calls = calls + 1 end,
            messageSpecial = function() calls = calls + 1 end,
            delCurrency = function() calls = calls + 1 end,
        }

        xi.campaign.sigilOnEventFinish(player, 13, utils.EVENT_CANCELLED_OPTION)

        assert(calls == 0)
    end)

    it('does not charge the free Regen-only selection', function()
        local originalGetMedalRank = xi.campaign.getMedalRank
        local debitCalls = 0
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            delStatusEffectsByFlag = function() end,
            addStatusEffect = function() end,
            messageSpecial = function() end,
            delCurrency = function() debitCalls = debitCalls + 1 end,
        }

        xi.campaign.getMedalRank = function() return 1 end
        xi.campaign.sigilOnEventFinish(player, 13, bit.bor(bit.lshift(1, 11), 1))

        assert(debitCalls == 0)
        xi.campaign.getMedalRank = originalGetMedalRank
    end)

    it('grants an affordable shop item before debiting its allied-note price', function()
        local granted, debited = nil, nil
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            getCampaignAllegiance = function() return xi.nation.BASTOK end,
            getCurrency = function() return 10 end,
            delCurrency = function(_, currency, amount) debited = { currency, amount } end,
        }

        stub('npcUtil.giveItem', function(actor, itemID)
            assert(actor == player)
            granted = itemID
            return true
        end)
        -- Common-page entry 1 is Scroll of Instant Retrace, priced at 10.
        xi.campaign.sigilOnEventFinish(player, 13, bit.bor(bit.lshift(1, 8), 2))

        assert(granted == xi.item.SCROLL_OF_INSTANT_RETRACE)
        assert(debited[1] == 'allied_notes' and debited[2] == 10)
    end)

    it('does not attempt a shop grant when allied notes are insufficient', function()
        local grantCalls, debitCalls = 0, 0
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            getCampaignAllegiance = function() return xi.nation.BASTOK end,
            getCurrency = function() return 9 end,
            delCurrency = function() debitCalls = debitCalls + 1 end,
        }

        stub('npcUtil.giveItem', function()
            grantCalls = grantCalls + 1
            return true
        end)
        xi.campaign.sigilOnEventFinish(player, 13, bit.bor(bit.lshift(1, 8), 2))

        assert(grantCalls == 0 and debitCalls == 0)
    end)
end)

describe('Campaign Sigil shop update', function()
    it('reports an otherwise-valid item as level-ineligible', function()
        local update = nil
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            canEquipItem = function(_, _, withLevel) return not withLevel end,
            updateEvent = function(_, ...) update = { ... } end,
        }

        stub('GetItemByID', {})
        stub('GetItemLevelRequirementsByID', 50)
        xi.campaign.sigilOnEventUpdate(player, 13, bit.bor(bit.lshift(2, 8), 2))

        assert(update[8] == 1)
    end)

    it('does not update a non-shop event', function()
        local updateCalls = 0
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            updateEvent = function() updateCalls = updateCalls + 1 end,
        }

        xi.campaign.sigilOnEventUpdate(player, 14, 2)

        assert(updateCalls == 0)
    end)
end)

describe('Campaign Sigil trigger', function()
    it('opens the no-medal dialog for a player without a campaign medal', function()
        local event = nil
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            hasKeyItem = function() return false end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.campaign.sigilOnTrigger(player)

        assert(#event == 1 and event[1] == 14)
    end)

    it('packs the active Sigil menu arguments for a medal holder', function()
        local event = nil
        local effect = { getTimeRemaining = function() return 5000 end }
        local player = {
            getZoneID = function() return xi.zone.BASTOK_MARKETS_S end,
            hasKeyItem = function(_, keyItem) return keyItem == xi.ki.BRONZE_RIBBON_OF_SERVICE end,
            getCampaignAllegiance = function() return xi.nation.BASTOK end,
            getCurrency = function() return 321 end,
            hasStatusEffect = function() return true end,
            getStatusEffect = function() return effect end,
            startEvent = function(_, ...) event = { ... } end,
        }

        stub('VanadielTime', 1000)
        xi.campaign.sigilOnTrigger(player)

        assert(#event == 9 and event[1] == 13)
        assert(event[2] == xi.nation.BASTOK and event[3] == 321 and event[4] == 0)
        assert(event[5] == 0 and event[6] == 1 and event[7] == 0)
        assert(event[8] == 1005 and event[9] == 0)
    end)
end)
