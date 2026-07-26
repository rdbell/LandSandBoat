-- Host-level coverage for xi.job_utils.warrior.useWarcry.
require('scripts/globals/job_utils/warrior')

describe('Warrior Warcry host', function()
    it('applies computed fields to a party target and changes the message', function()
        local applied
        local message
        local player = {
            getMainJob = function()
                return xi.job.WAR
            end,
            getSubJob = function()
                return 0
            end,
            getMainLvl = function()
                return 75
            end,
            getSubLvl = function()
                return 0
            end,
            getMod = function()
                return 12
            end,
            getMerit = function()
                return 25
            end,
            getID = function()
                return 1
            end,
        }
        local target = {
            getID = function()
                return 2
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }
        local ability = {
            setMsg = function(_, msg)
                message = msg
            end,
        }

        assert(xi.job_utils.warrior.useWarcry(player, target, ability) == xi.effect.WARCRY)

        assert(applied.effect == xi.effect.WARCRY)
        assert(math.abs(applied.params.power - ((23 / 256) * 100)) < 1e-9)
        assert(applied.params.duration == 42 and applied.params.subPower == 25)
        assert(applied.params.origin == player)
        assert(message == xi.msg.basic.JA_ATK_ENHANCED)
    end)

    it('retains the default message for a self target', function()
        local messageChanged = false
        local player = {
            getMainJob = function()
                return xi.job.WAR
            end,
            getSubJob = function()
                return 0
            end,
            getMainLvl = function()
                return 1
            end,
            getSubLvl = function()
                return 0
            end,
            getMod = function()
                return 0
            end,
            getMerit = function()
                return 0
            end,
            getID = function()
                return 1
            end,
            addStatusEffect = function()
            end,
        }
        local ability = {
            setMsg = function()
                messageChanged = true
            end,
        }

        assert(xi.job_utils.warrior.useWarcry(player, player, ability) == xi.effect.WARCRY)
        assert(not messageChanged)
    end)
end)
