require('scripts/globals/job_utils/samurai')

describe('Samurai Shikikoyo host', function()
    it('resets actor TP and transfers its capped amount to the target', function()
        local playerTP
        local targetTP
        local player = {
            getTP = function()
                return 3000
            end,
            getMerit = function()
                return 12
            end,
            setTP = function(_, value)
                playerTP = value
            end,
        }
        local target = {
            getTP = function()
                return 2500
            end,
            setTP = function(_, value)
                targetTP = value
            end,
        }

        assert(xi.job_utils.samurai.useShikikoyo(player, target, {}, {}) == 500)
        assert(playerTP == 1000 and targetTP == 3000)
    end)
end)
