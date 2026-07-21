-----------------------------------
-- Pure system tests for npcUtil.openCrate pure plan (slice 6173).
-----------------------------------

describe('npcutil openCrate pure plan', function()
    local DISAPPEAR_MS = 3000
    local OPEN_THEN_DISAPPEAR_MS = 7000

    -- Can open only when opened local var is 0.
    local function canOpen(openedVar)
        return openedVar == 0
    end

    -- After open: set opened=1; shouldDisappear = not callbackKeepOpen
    local function planOpen(openedVar, callbackKeepOpen)
        if not canOpen(openedVar) then
            return { opened = false }
        end
        local shouldDisappear = not callbackKeepOpen
        return {
            opened = true,
            setOpened = 1,
            shouldDisappear = shouldDisappear,
            disappearDelayMs = shouldDisappear and OPEN_THEN_DISAPPEAR_MS or nil,
        }
    end

    it('rejects already-opened crate', function()
        assert(not canOpen(1))
        local r = planOpen(1, false)
        assert(not r.opened)
    end)

    it('opens and schedules disappear when callback returns false', function()
        local r = planOpen(0, false)
        assert(r.opened and r.setOpened == 1 and r.shouldDisappear)
        assert(r.disappearDelayMs == 7000)
    end)

    it('opens and keeps crate when callback returns true', function()
        local r = planOpen(0, true)
        assert(r.opened and r.setOpened == 1 and not r.shouldDisappear)
        assert(r.disappearDelayMs == nil)
    end)

    it('pins disappear timer constants', function()
        assert(DISAPPEAR_MS == 3000)
        assert(OPEN_THEN_DISAPPEAR_MS == 7000)
    end)
end)
