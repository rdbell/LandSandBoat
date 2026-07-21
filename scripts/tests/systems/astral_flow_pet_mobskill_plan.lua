-----------------------------------
-- Pure system tests for Astral Flow pet (slice 6143).
-----------------------------------

describe('Astral Flow pet pure plan', function()
    local ELEMENTAL = 10
    local catalog = {
        [848] = { 248, 383 }, -- Inferno / Ifrit
        [912] = { 243, 379 }, -- Searing Light / Carbuncle
        [839] = { 246, 381 }, -- Howling Moon / Fenrir
    }

    local function abilityFor(family)
        for skillId, families in pairs(catalog) do
            for _, f in ipairs(families) do
                if f == family then return skillId end
            end
        end
        return 912
    end

    local function skillCheck(p)
        if not p.hasPet or p.ecosystem ~= ELEMENTAL or p.inactive then
            return 1
        end
        return 0
    end

    it('skill check gates', function()
        assert(skillCheck({ hasPet = false }) == 1)
        assert(skillCheck({ hasPet = true, ecosystem = 1 }) == 1)
        assert(skillCheck({ hasPet = true, ecosystem = ELEMENTAL, inactive = true }) == 1)
        assert(skillCheck({ hasPet = true, ecosystem = ELEMENTAL }) == 0)
    end)

    it('maps species to ability', function()
        assert(abilityFor(248) == 848)
        assert(abilityFor(999) == 912)
    end)
end)
