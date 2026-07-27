require('scripts/globals/job_utils/paladin')

describe('Paladin Sentinel', function()
    it('adds the enhanced Sentinel effect with tick and sub-power', function()
        local effect
        local player = {
            getMod = function(_, mod)
                if mod == xi.mod.SENTINEL_EFFECT then
                    return 10
                end
                assert(mod == xi.mod.ENHANCES_GUARDIAN)
                return 4
            end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.GUARDIAN)
                return 19
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.SENTINEL_EFFECT)
                return 7
            end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.paladin.useSentinel(player, {}, {})

        assert(result == xi.effect.SENTINEL and effect.id == xi.effect.SENTINEL)
        assert(effect.values.power == 10000 and effect.values.duration == 34 and effect.values.tick == 3 and effect.values.subPower == 26 and effect.values.origin == player)
    end)

    it('preserves a fractional Guardian duration bonus', function()
        local duration
        local player = {
            getMod = function(_, mod)
                if mod == xi.mod.SENTINEL_EFFECT then return 0 end
                assert(mod == xi.mod.ENHANCES_GUARDIAN)
                return 4
            end,
            getMerit = function(_, merit) assert(merit == xi.merit.GUARDIAN); return 10 end,
            getJobPointLevel = function(_, jp) assert(jp == xi.jp.SENTINEL_EFFECT); return 0 end,
            addStatusEffect = function(_, _, values) duration = values.duration end,
        }

        xi.job_utils.paladin.useSentinel(player, {}, {})

        assert(duration == 30 + 40 / 19)
    end)
end)
