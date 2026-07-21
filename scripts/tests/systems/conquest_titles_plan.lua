-----------------------------------
-- Pure system tests for conquest titlesGranted catalog (slice 6201).
-----------------------------------

describe('conquest titlesGranted pure plan', function()
    local SANDORIA, BASTOK, WINDURST = 0, 1, 2

    -- xi.title pins
    local titlesGranted =
    {
        [0] = { -- San d'Oria
            [1] = 226, [2] = 227, [3] = 228, [4] = 229, [5] = 230,
            [6] = 231, [7] = 232, [8] = 233, [9] = 234, [10] = 235,
        },
        [1] = { -- Bastok
            [1] = 236, [2] = 237, [3] = 238, [4] = 239, [5] = 240,
            [6] = 241, [7] = 242, [8] = 243, [9] = 244, [10] = 245,
        },
        [2] = { -- Windurst
            [1] = 246, [2] = 247, [3] = 248, [4] = 249, [5] = 250,
            [6] = 251, [7] = 252, [8] = 253, [9] = 254, [10] = 255,
        },
    }

    local function planTitle(nation, rank)
        local byNation = titlesGranted[nation]
        if not byNation then
            return nil
        end
        return byNation[rank]
    end

    it('catalog shape', function()
        for nation = 0, 2 do
            for rank = 1, 10 do
                assert(titlesGranted[nation][rank] ~= nil)
            end
        end
    end)

    it('sample pins', function()
        assert(planTitle(SANDORIA, 1) == 226) -- ROYAL_ARCHER
        assert(planTitle(SANDORIA, 10) == 235) -- ELITE_ROYAL_GUARD
        assert(planTitle(BASTOK, 1) == 236) -- LEGIONNAIRE
        assert(planTitle(BASTOK, 10) == 245) -- PRAEFECTUS_CASTRORUM
        assert(planTitle(WINDURST, 1) == 246) -- FREESWORD
        assert(planTitle(WINDURST, 10) == 255) -- MERCENARY_MAJOR
        assert(planTitle(3, 1) == nil)
        assert(planTitle(SANDORIA, 11) == nil)
        assert(planTitle(SANDORIA, 0) == nil)
    end)

    it('contiguous title ids per nation', function()
        assert(titlesGranted[0][10] - titlesGranted[0][1] == 9)
        assert(titlesGranted[1][10] - titlesGranted[1][1] == 9)
        assert(titlesGranted[2][10] - titlesGranted[2][1] == 9)
    end)
end)
