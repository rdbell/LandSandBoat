require('scripts/globals/job_utils/bard')

describe('Bard Clarion Call', function()
    it('adds the fixed Clarion Call effect and returns its ID', function()
        local effect
        local player = {
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.bard.useClarionCall(player, {}, {})

        assert(result == xi.effect.CLARION_CALL and effect.id == xi.effect.CLARION_CALL)
        assert(effect.values.power == 10 and effect.values.duration == 180 and effect.values.origin == player)
    end)
end)
