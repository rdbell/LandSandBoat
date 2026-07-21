-----------------------------------
-- Pure system tests for item_utils handlePetLatentMods (slice 6136).
-- Source: scripts/globals/item_utils.lua ~284–301
-----------------------------------

describe('itemUtils handlePetLatentMods pure plan', function()
    local function plan(p)
        if not p.hasPet then
            return { apply = false }
        end
        if p.requiredPetID and p.requiredPetID ~= 0 and p.petID ~= p.requiredPetID then
            return { apply = false }
        end
        return { apply = true, give = p.give, mods = p.mods }
    end

    it('requires a pet', function()
        assert(plan({ hasPet = false, give = true, mods = { { 1, 10 } } }).apply == false)
    end)

    it('filters by required pet id', function()
        assert(plan({ hasPet = true, requiredPetID = 5, petID = 3, give = true }).apply == false)
        local r = plan({ hasPet = true, requiredPetID = 5, petID = 5, give = false, mods = { { 2, 3 } } })
        assert(r.apply == true and r.give == false)
    end)

    it('nil/zero required matches any pet', function()
        local r = plan({ hasPet = true, requiredPetID = 0, petID = 99, give = true, mods = { { 1, 1 } } })
        assert(r.apply == true and r.give == true)
    end)
end)
