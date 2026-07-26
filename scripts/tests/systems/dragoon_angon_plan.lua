require('scripts/globals/job_utils/dragoon')

describe('Dragoon Angon plan', function()
    local function runAngon(effectApplied)
        local statusParams
        local claimUpdated = false
        local ammoRemoved
        local message
        local player = {
            getMerit = function(_, merit)
                assert(merit == xi.merit.ANGON)
                return 15
            end,
            removeAmmo = function(_, count) ammoRemoved = count end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.DEFENSE_DOWN)
                statusParams = params
                return effectApplied
            end,
            updateClaim = function(_, claimant)
                assert(claimant == player)
                claimUpdated = true
            end,
        }
        local ability = { setMsg = function(_, msg) message = msg end }

        assert(xi.job_utils.dragoon.useAngon(player, target, ability) == xi.effect.DEFENSE_DOWN)
        assert(statusParams.power == 20)
        assert(statusParams.duration == 30)
        assert(statusParams.origin == player)
        assert(claimUpdated)
        assert(ammoRemoved == 1)
        return message
    end

    it('applies Defense Down and consumes one Angon', function()
        assert(runAngon(true) == nil)
    end)

    it('reports no effect but still claims and consumes Angon', function()
        assert(runAngon(false) == xi.msg.basic.MAGIC_NO_EFFECT)
    end)
end)
