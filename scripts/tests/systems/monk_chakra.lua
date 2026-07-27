require('scripts/globals/job_utils/monk')

describe('Monk Chakra', function()
    it('clears eligible effects, restores HP, and replaces Invigorate Regen', function()
        local originalActiveLevel = utils.getActiveJobLevel
        utils.getActiveJobLevel = function(_, job)
            assert(job == xi.job.MNK)
            return 99
        end

        local removed = {}
        local setHP
        local regen
        local player = {
            getMod = function(_, mod)
                if mod == xi.mod.CHAKRA_REMOVAL then
                    return 1 + 2 + 4
                end
                assert(mod == xi.mod.CHAKRA_MULT)
                return 0
            end,
            delStatusEffect = function(_, effect)
                table.insert(removed, effect)
            end,
            getStat = function(_, mod)
                assert(mod == xi.mod.VIT)
                return 100
            end,
            getMaxHP = function()
                return 2000
            end,
            getHP = function()
                return 1000
            end,
            setHP = function(_, hp)
                setHP = hp
            end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.INVIGORATE)
                return 40
            end,
            hasStatusEffect = function(_, effect)
                assert(effect == xi.effect.REGEN)
                return true
            end,
            addStatusEffect = function(_, effect, values)
                regen = { effect = effect, values = values }
            end,
        }
        local target = {
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.CHAKRA_EFFECT)
                return 30
            end,
        }

        local recovery = xi.job_utils.monk.useChakra(player, target, {})
        utils.getActiveJobLevel = originalActiveLevel

        assert(recovery == 630 and setHP == 1630)
        assert(#removed == 6)
        assert(removed[1] == xi.effect.POISON and removed[2] == xi.effect.BLINDNESS)
        assert(removed[3] == xi.effect.PARALYSIS and removed[4] == xi.effect.DISEASE and removed[5] == xi.effect.PLAGUE)
        assert(removed[6] == xi.effect.REGEN)
        assert(regen.effect == xi.effect.REGEN and regen.values.power == 10 and regen.values.duration == 40 and regen.values.origin == player and regen.values.tier == 1)
    end)
end)
