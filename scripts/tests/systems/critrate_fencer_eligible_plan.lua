-----------------------------------
-- Pure system tests for Fencer gear eligibility (slice 6212).
-----------------------------------

describe('critrate fencer eligible pure plan', function()
    local function fencerEligible(isPC, hasMain, main2H, mainH2H, hasSub, subSkill, subShield)
        if not isPC or not hasMain then
            return false
        end
        if main2H or mainH2H then
            return false
        end
        if not hasSub then
            return true
        end
        if subSkill == 0 then
            return true
        end
        return subShield
    end

    it('one-hand + empty/shield/none sub is eligible', function()
        assert(fencerEligible(true, true, false, false, false, 0, false))
        assert(fencerEligible(true, true, false, false, true, 0, false))
        assert(fencerEligible(true, true, false, false, true, 32, true))
    end)

    it('two-hand, H2H, dual wield, non-PC are not', function()
        assert(not fencerEligible(true, true, true, false, false, 0, false))
        assert(not fencerEligible(true, true, false, true, false, 0, false))
        assert(not fencerEligible(true, true, false, false, true, 3, false))
        assert(not fencerEligible(false, true, false, false, false, 0, false))
        assert(not fencerEligible(true, false, false, false, false, 0, false))
    end)
end)
