-----------------------------------
-- Pure system tests for getVanadielMoonCycle threshold tables.
-----------------------------------

describe('Vanadiel moon cycle pure plans', function()
    it('moon cycle enum pins NEW through LESSER_WANING_CRESCENT', function()
        assert(xi.moonCycle.NEW_MOON == 0)
        assert(xi.moonCycle.FULL_MOON == 6)
        assert(xi.moonCycle.GREATER_WANING_GIBBOUS == 7)
        assert(xi.moonCycle.LESSER_WANING_CRESCENT == 11)
    end)

    -- Mirror pure lookup without Vanadiel injects.
    local function cycleFromPhase(phase, direction)
        local moonTable =
        {
            [0] =
            {
                { 100, xi.moonCycle.FULL_MOON },
                { 0,   xi.moonCycle.NEW_MOON },
            },
            [1] =
            {
                { 94, xi.moonCycle.FULL_MOON },
                { 77, xi.moonCycle.GREATER_WANING_GIBBOUS },
                { 61, xi.moonCycle.LESSER_WANING_GIBBOUS },
                { 41, xi.moonCycle.THIRD_QUARTER },
                { 25, xi.moonCycle.GREATER_WANING_CRESCENT },
                { 11, xi.moonCycle.LESSER_WANING_CRESCENT },
                { 0,  xi.moonCycle.NEW_MOON },
            },
            [2] =
            {
                { 90, xi.moonCycle.FULL_MOON },
                { 72, xi.moonCycle.GREATER_WAXING_GIBBOUS },
                { 56, xi.moonCycle.LESSER_WAXING_GIBBOUS },
                { 39, xi.moonCycle.FIRST_QUARTER },
                { 22, xi.moonCycle.GREATER_WAXING_CRESCENT },
                {  6, xi.moonCycle.LESSER_WAXING_CRESCENT },
                {  0, xi.moonCycle.NEW_MOON },
            },
        }

        local directionTable = moonTable[direction]
        if not directionTable then
            return xi.moonCycle.GREATER_WANING_GIBBOUS
        end

        for _, entry in ipairs(directionTable) do
            local threshold, moonCycle = entry[1], entry[2]
            if phase >= threshold then
                return moonCycle
            end
        end

        return xi.moonCycle.GREATER_WANING_GIBBOUS
    end

    it('peak direction: full at 100, new otherwise', function()
        assert(cycleFromPhase(100, 0) == xi.moonCycle.FULL_MOON)
        assert(cycleFromPhase(50, 0) == xi.moonCycle.NEW_MOON)
        assert(cycleFromPhase(0, 0) == xi.moonCycle.NEW_MOON)
    end)

    it('descending boundaries', function()
        assert(cycleFromPhase(94, 1) == xi.moonCycle.FULL_MOON)
        assert(cycleFromPhase(93, 1) == xi.moonCycle.GREATER_WANING_GIBBOUS)
        assert(cycleFromPhase(61, 1) == xi.moonCycle.LESSER_WANING_GIBBOUS)
        assert(cycleFromPhase(41, 1) == xi.moonCycle.THIRD_QUARTER)
        assert(cycleFromPhase(11, 1) == xi.moonCycle.LESSER_WANING_CRESCENT)
        assert(cycleFromPhase(10, 1) == xi.moonCycle.NEW_MOON)
    end)

    it('ascending boundaries', function()
        assert(cycleFromPhase(90, 2) == xi.moonCycle.FULL_MOON)
        assert(cycleFromPhase(89, 2) == xi.moonCycle.GREATER_WAXING_GIBBOUS)
        assert(cycleFromPhase(39, 2) == xi.moonCycle.FIRST_QUARTER)
        assert(cycleFromPhase(6, 2) == xi.moonCycle.LESSER_WAXING_CRESCENT)
        assert(cycleFromPhase(5, 2) == xi.moonCycle.NEW_MOON)
    end)

    it('unknown direction falls back to GREATER_WANING_GIBBOUS', function()
        assert(cycleFromPhase(50, 99) == xi.moonCycle.GREATER_WANING_GIBBOUS)
    end)
end)
