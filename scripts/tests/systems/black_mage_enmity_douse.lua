require('scripts/globals/job_utils/black_mage')

describe('Black Mage Enmity Douse', function()
    it('sets a mob target CE to 1 and VE to 0 for the player', function()
        local calls = {}
        local player = {}
        local target = {
            isMob = function() return true end,
            setCE = function(_, source, value) calls.ce = { source = source, value = value } end,
            setVE = function(_, source, value) calls.ve = { source = source, value = value } end,
        }

        local result = xi.job_utils.black_mage.useEnmityDouse(player, target, {})

        assert(result == nil)
        assert(calls.ce.source == player and calls.ce.value == 1)
        assert(calls.ve.source == player and calls.ve.value == 0)
    end)

    it('does not change enmity on a non-mob target', function()
        local calls = 0
        local target = {
            isMob = function() return false end,
            setCE = function() calls = calls + 1 end,
            setVE = function() calls = calls + 1 end,
        }

        xi.job_utils.black_mage.useEnmityDouse({}, target, {})

        assert(calls == 0)
    end)
end)
