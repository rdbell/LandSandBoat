require('scripts/globals/job_utils/paladin')

describe('Paladin Chivalry', function()
    it('resets target TP, adds the calculated MP, and returns the MP result', function()
        local resetTP
        local addedMP
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.ENHANCES_CHIVALRY)
                return 0
            end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.CHIVALRY)
                return 5
            end,
        }
        local target = {
            getTP = function() return 1000 end,
            getStat = function(_, mod) assert(mod == xi.mod.MND); return 100 end,
            setTP = function(_, value) resetTP = value end,
            addMP = function(_, value) addedMP = value; return 123 end,
        }

        local result = xi.job_utils.paladin.useChivalry(player, target, {}, {})

        assert(resetTP == 0 and addedMP == 200 and result == 123)
    end)
end)
