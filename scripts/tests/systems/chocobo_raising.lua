require('scripts/globals/hobbies/chocobo_raising/condense_events')

describe('Chocobo Raising event condenser', function()
    it('returns no spans for no events', function()
        assert(#xi.chocoboRaising.condenseEvents({}) == 0)
    end)

    it('appends cutscenes that occur on the same day', function()
        local spans = xi.chocoboRaising.condenseEvents({
            { 4, { 10 } },
            { 4, { 11, 12 } },
        })

        assert(#spans == 1)
        assert(spans[1][1] == 4 and spans[1][2] == 4)
        assert(#spans[1][3] == 3)
        assert(spans[1][3][1] == 10 and spans[1][3][2] == 11 and spans[1][3][3] == 12)
    end)

    it('collapses contiguous days with identical cutscene lists', function()
        local spans = xi.chocoboRaising.condenseEvents({
            { 4, { 10, 11 } },
            { 5, { 10, 11 } },
        })

        assert(#spans == 1)
        assert(spans[1][1] == 4 and spans[1][2] == 5)
        assert(#spans[1][3] == 2)
        assert(spans[1][3][1] == 10 and spans[1][3][2] == 11)
    end)

    it('starts a new span when the cutscene list changes', function()
        local spans = xi.chocoboRaising.condenseEvents({
            { 4, { 10 } },
            { 5, { 11 } },
            { 6, { 11 } },
        })

        assert(#spans == 2)
        assert(spans[1][1] == 4 and spans[1][2] == 4 and spans[1][3][1] == 10)
        assert(spans[2][1] == 5 and spans[2][2] == 6 and spans[2][3][1] == 11)
    end)

    it('stops after the Adult 3 to Adult 4 retirement cutscene', function()
        local spans = xi.chocoboRaising.condenseEvents({
            { 4, { 10 } },
            { 5, { xi.chocoboRaising.cutscenes.ADULT_3_TO_ADULT_4 } },
            { 6, { 11 } },
        })

        assert(#spans == 2)
        assert(spans[2][1] == 5 and spans[2][2] == 5)
        assert(spans[2][3][1] == xi.chocoboRaising.cutscenes.ADULT_3_TO_ADULT_4)
    end)
end)
