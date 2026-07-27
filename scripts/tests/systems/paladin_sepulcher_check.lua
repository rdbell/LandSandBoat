require('scripts/globals/job_utils/paladin')

describe('Paladin Sepulcher check', function()
    it('allows undead targets and rejects other targets', function()
        local undead = { isUndead = function() return true end }
        local living = { isUndead = function() return false end }

        local msg, status = xi.job_utils.paladin.checkSepulcher({}, undead, {})
        assert(msg == 0 and status == 0)

        msg, status = xi.job_utils.paladin.checkSepulcher({}, living, {})
        assert(msg == xi.msg.basic.CANNOT_ON_THAT_TARG and status == 0)
    end)
end)
