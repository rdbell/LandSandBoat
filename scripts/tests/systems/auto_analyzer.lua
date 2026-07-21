-----------------------------------
-- Pure system tests for utils.handleAutomatonAutoAnalyzer inject form.
-----------------------------------

describe('auto analyzer pure plans', function()
    local function handle(damage, analyzerMod, incomingSkill, analyzed)
        if analyzerMod <= 0 then
            return damage
        end

        local count = math.min(analyzerMod, 6)
        for i = 1, count do
            if incomingSkill == analyzed[i] then
                return math.floor(damage * 0.6)
            end
        end

        return damage
    end

    it('returns damage when analyzer mod inactive', function()
        assert(handle(100, 0, 5, { 5 }) == 100)
    end)

    it('reduces matching skill to 60%', function()
        assert(handle(100, 2, 42, { 42, 99 }) == 60)
        assert(handle(101, 1, 1, { 1 }) == 60)
    end)

    it('ignores skills beyond min(mod, 6)', function()
        local analyzed = { 1, 2, 3, 4, 5, 6, 7 }
        assert(handle(100, 10, 7, analyzed) == 100)
        assert(handle(100, 10, 6, analyzed) == 60)
    end)
end)
