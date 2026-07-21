-----------------------------------
-- Pure system tests for job innate-caster catalog (jobs.lua).
-----------------------------------

describe('Job innate caster pure plans', function()
    it('dataTable marks caster jobs true', function()
        local casters = {
            xi.job.WHM, xi.job.BLM, xi.job.RDM, xi.job.PLD, xi.job.DRK,
            xi.job.BRD, xi.job.NIN, xi.job.SMN, xi.job.BLU,
            xi.job.SCH, xi.job.GEO, xi.job.RUN,
        }
        for _, j in ipairs(casters) do
            assert(xi.data.job.dataTable[j][1] == true, 'job ' .. j)
        end
    end)

    it('dataTable marks non-casters false', function()
        local non = {
            xi.job.NONE, xi.job.WAR, xi.job.MNK, xi.job.THF, xi.job.BST,
            xi.job.RNG, xi.job.SAM, xi.job.DRG, xi.job.COR, xi.job.PUP, xi.job.DNC,
        }
        for _, j in ipairs(non) do
            assert(xi.data.job.dataTable[j][1] == false, 'job ' .. j)
        end
    end)

    it('isInnateCaster uses main or sub job', function()
        local function stub(main, sub)
            return {
                getMainJob = function() return main end,
                getSubJob = function() return sub end,
            }
        end
        assert(not xi.data.job.isInnateCaster(stub(xi.job.WAR, xi.job.THF)))
        assert(xi.data.job.isInnateCaster(stub(xi.job.WAR, xi.job.WHM)))
        assert(xi.data.job.isInnateCaster(stub(xi.job.BLM, xi.job.WAR)))
        assert(xi.data.job.isInnateCaster(stub(xi.job.NIN, xi.job.WAR)))
        assert(not xi.data.job.isInnateCaster(stub(xi.job.COR, xi.job.PUP)))
        assert(not xi.data.job.isInnateCaster(stub(xi.job.NONE, xi.job.NONE)))
    end)

    it('job enum pins match catalog keys', function()
        assert(xi.job.NONE == 0 and xi.job.WAR == 1 and xi.job.RUN == 22)
        assert(xi.job.WHM == 3 and xi.job.NIN == 13 and xi.job.SCH == 20)
    end)
end)
