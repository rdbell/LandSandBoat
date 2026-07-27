require('scripts/globals/apkallu')

describe('Apkallu ability admission', function()
    local function canUse(isNM, zone, hate, threshold)
        local nm = isNM
        local mob = {
            isNM = function() return nm end,
            getZoneID = function() return zone end,
        }
        SetServerVariable('ApkalluHate_'..zone, hate)
        local result = xi.apkallu.canUseAbility(mob, threshold)
        return result
    end

    it('admits only ordinary Apkallu in the two hate-controlled zones below threshold', function()
        assert(canUse(false, xi.zone.ARRAPAGO_REEF, 19, 20) == 1)
        assert(canUse(false, xi.zone.MOUNT_ZHAYOLM, 0, 20) == 1)
        assert(canUse(false, xi.zone.ARRAPAGO_REEF, 20, 20) == 0)
        assert(canUse(false, xi.zone.ARRAPAGO_REEF, 21, 20) == 0)
        assert(canUse(true, xi.zone.ARRAPAGO_REEF, 0, 20) == 0)
        assert(canUse(false, 0, 0, 20) == 0)
    end)
end)
