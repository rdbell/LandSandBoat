require('scripts/globals/abyssea')

describe('Abyssea time remaining', function()
    local function player(milliseconds)
        local messages = {}

        return {
            getZoneID = function()
                return xi.zone.ABYSSEA_ALTEPA
            end,
            getStatusEffect = function()
                return {
                    getTimeRemaining = function()
                        return milliseconds
                    end,
                }
            end,
            messageSpecial = function(_, ...)
                table.insert(messages, { ... })
            end,
            messages = function()
                return messages
            end,
        }
    end

    it('uses the plural-minute message at two minutes', function()
        local p = player(120000)
        xi.abyssea.displayTimeRemaining(p)
        assert(p.messages()[1][1] == 7348 and p.messages()[1][2] == 2 and p.messages()[1][3] == 1)
    end)

    it('uses the singular-minute message below two minutes', function()
        local p = player(60000)
        xi.abyssea.displayTimeRemaining(p)
        assert(p.messages()[1][1] == 7347 and p.messages()[1][2] == 1 and p.messages()[1][3] == 1)
    end)

    it('uses the seconds message below one minute', function()
        local p = player(59000)
        xi.abyssea.displayTimeRemaining(p)
        assert(p.messages()[1][1] == 7348 and p.messages()[1][2] == 59 and p.messages()[1][3] == 0)
    end)
end)
