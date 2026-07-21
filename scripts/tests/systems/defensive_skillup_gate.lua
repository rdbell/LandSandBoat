-----------------------------------
-- Pure system tests for defensive skill-up try gate (slice 6096).
-- Shared by isParried / isGuarded / isBlocked in physical_utilities.lua.
-----------------------------------

describe('defensive skill-up try gate pure injects', function()
    local function shouldTry(isPC, success, oldStyle)
        return isPC and (success or not oldStyle)
    end

    it('non-PC never skills up', function()
        assert(not shouldTry(false, true, false))
        assert(not shouldTry(false, true, true))
    end)

    it('old style only on success', function()
        assert(shouldTry(true, true, true))
        assert(not shouldTry(true, false, true))
    end)

    it('new style always for PC', function()
        assert(shouldTry(true, true, false))
        assert(shouldTry(true, false, false))
    end)
end)
