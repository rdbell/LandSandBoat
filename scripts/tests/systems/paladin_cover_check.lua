require('scripts/globals/job_utils/paladin')

describe('Paladin Cover check', function()
    it('rejects nil, self, and non-PC targets', function()
        local player = { getID = function() return 100 end }
        local cases = {
            nil,
            { getID = function() return 100 end, isPC = function() return true end },
            { getID = function() return 200 end, isPC = function() return false end },
        }

        for _, target in ipairs(cases) do
            local msg, status = xi.job_utils.paladin.checkCover(player, target, {})
            assert(msg == xi.msg.basic.CANNOT_PERFORM_TARG and status == 0)
        end
        local msg, status = xi.job_utils.paladin.checkCover(player, nil, {})
        assert(msg == xi.msg.basic.CANNOT_PERFORM_TARG and status == 0)
    end)

    it('allows a distinct PC target', function()
        local player = { getID = function() return 100 end }
        local target = { getID = function() return 200 end, isPC = function() return true end }

        local msg, status = xi.job_utils.paladin.checkCover(player, target, {})
        assert(msg == 0 and status == 0)
    end)
end)
