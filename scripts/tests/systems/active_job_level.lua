-----------------------------------
-- Pure system tests for utils.getActiveJobLevel inject form.
-----------------------------------

describe('getActiveJobLevel pure plans', function()
    -- Mirror pure inject without entity.
    local function activeJobLevel(mainJob, subJob, job, mainLevel, subLevel)
        if mainJob == job then
            return mainLevel
        elseif subJob == job then
            return subLevel
        end

        return 0
    end

    it('returns main level when main job matches', function()
        assert(activeJobLevel(1, 2, 1, 75, 37) == 75)
    end)

    it('returns sub level when sub job matches', function()
        assert(activeJobLevel(2, 1, 1, 75, 37) == 37)
    end)

    it('returns 0 when neither job matches', function()
        assert(activeJobLevel(2, 3, 1, 75, 37) == 0)
    end)

    it('main takes priority over sub when both match', function()
        -- same job as main and sub (unusual but pure path)
        assert(activeJobLevel(1, 1, 1, 99, 49) == 99)
    end)
end)
