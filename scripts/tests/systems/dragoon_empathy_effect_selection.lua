require('scripts/globals/job_utils/dragoon')

describe('Dragoon Empathy effect selection', function()
    it('randomly removes effects until the merit copy limit remains', function()
        local random = math.random
        local picks = { 2, 1 }
        math.random = function()
            local pick = picks[1]
            table.remove(picks, 1)
            return pick
        end

        local effects = xi.job_utils.dragoon.cutEmpathyEffectTable({ 10, 20, 30, 40 }, 4, 2)
        math.random = random

        assert(#effects == 4)
        assert(effects[1] == 30 and effects[2] == 40 and effects[3] == 40 and effects[4] == 40)
    end)

    it('preserves the full table when it is already within the limit', function()
        local effects = xi.job_utils.dragoon.cutEmpathyEffectTable({ 10, 20 }, 2, 2)
        assert(#effects == 2)
        assert(effects[1] == 10 and effects[2] == 20)
    end)
end)
