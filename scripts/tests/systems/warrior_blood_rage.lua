-- Host-level coverage for xi.job_utils.warrior.useBloodRage.
require('scripts/globals/job_utils/warrior')

describe('Warrior Blood Rage host', function()
    it('applies the computed effect to a party target and changes the message', function()
        local applied
        local message
        local player = {
            getJobPointLevel = function()
                return 10
            end,
            getMod = function()
                return 15
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

        assert(xi.job_utils.warrior.useBloodRage(player, target, ability) == xi.effect.BLOOD_RAGE)
        assert(applied.effect == xi.effect.BLOOD_RAGE)
        assert(applied.params.power == 30 and applied.params.duration == 45)
        assert(applied.params.origin == player)
        assert(message == xi.msg.basic.JA_GAIN_EFFECT)
    end)

    it('retains the default message for a self target', function()
        local applied
        local messageChanged = false
        local player = {
            getJobPointLevel = function()
                return 0
            end,
            getMod = function()
                return 0
            end,
            getID = function()
                return 1
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }
        local ability = {
            setMsg = function()
                messageChanged = true
            end,
        }

        assert(xi.job_utils.warrior.useBloodRage(player, player, ability) == xi.effect.BLOOD_RAGE)
        assert(applied.effect == xi.effect.BLOOD_RAGE)
        assert(applied.params.power == 20 and applied.params.duration == 30)
        assert(applied.params.origin == player)
        assert(not messageChanged)
    end)
end)
