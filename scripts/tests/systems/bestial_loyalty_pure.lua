-----------------------------------
-- Pure Bestial Loyalty admission checks (slice 9260).
-- Goldens match internal/beastmaster.
-----------------------------------

require('scripts/globals/job_utils/beastmaster')

local b = xi.job_utils.beastmaster

describe('Bestial Loyalty check', function()
    it('pins rejection messages and zero parameters', function()
        local msg, param = b.checkBestialLoyaltyFromParams({
            hasPet = true, jugValid = false, canUsePet = false,
        })
        assert(msg == 315 and param == 0)

        msg, param = b.checkBestialLoyaltyFromParams({
            hasPet = false, jugValid = false, canUsePet = false,
        })
        assert(msg == 337 and param == 0)

        msg, param = b.checkBestialLoyaltyFromParams({
            hasPet = false, jugValid = true, canUsePet = false,
        })
        assert(msg == 316 and param == 0)

        msg, param = b.checkBestialLoyaltyFromParams({
            hasPet = false, jugValid = true, canUsePet = true,
        })
        assert(msg == 0 and param == 0)
    end)

    it('keeps the existing-pet precedence over every later rejection', function()
        local msg = b.checkBestialLoyaltyFromParams({
            hasPet = true, jugValid = true, canUsePet = false,
        })
        assert(msg == 315)
    end)
end)
