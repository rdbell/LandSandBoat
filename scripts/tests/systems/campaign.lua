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
end)
