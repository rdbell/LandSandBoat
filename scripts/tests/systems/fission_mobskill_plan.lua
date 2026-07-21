-----------------------------------
-- Pure system tests for Fission (slice 6142).
-----------------------------------

describe('Fission pure plan', function()
    local function petIDs(mobID, numAdds, offset)
        local pets = {}
        for i = 1, numAdds do
            pets[i] = mobID + offset + i
        end
        return pets
    end

    it('builds pet id list', function()
        local p = petIDs(1000, 3, 10)
        assert(p[1] == 1011 and p[2] == 1012 and p[3] == 1013)
    end)

    it('empty when numAdds < 1', function()
        assert(#petIDs(100, 0, 5) == 0)
    end)
end)
