require('scripts/globals/job_utils/paladin')

describe('Paladin Shield Bash check', function()
    it('requires a shield and otherwise allows the ability', function()
        local noShield = { getShieldSize = function() return 0 end }
        local shield = { getShieldSize = function() return 1 end }

        local msg, status = xi.job_utils.paladin.checkShieldBash(noShield, {}, {})
        assert(msg == xi.msg.basic.REQUIRES_SHIELD and status == 0)

        msg, status = xi.job_utils.paladin.checkShieldBash(shield, {}, {})
        assert(msg == 0 and status == 0)
    end)
end)
