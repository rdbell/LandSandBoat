require('scripts/globals/chocobo_names')

describe('Chocobo random name', function()
    it('retries catalog gaps and draws only five-to-eight-letter indices', function()
        local originalRandom = math.random
        local picks = { 503, 168, 762 }
        local calls = 0

        math.random = function(low, high)
            calls = calls + 1
            assert(low == 168 and high == 762)
            return table.remove(picks, 1)
        end

        local ok, first, second = pcall(function()
            return xi.chocoboNames.getRandomName(), xi.chocoboNames.getRandomName()
        end)
        math.random = originalRandom

        assert(ok)
        assert(first == xi.chocoboNames[168])
        assert(second == xi.chocoboNames[762])
        assert(calls == 3)
    end)
end)
