require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Link enhanced cleanse', function()
    it('randomly removes up to two erasable or -na effects with enhancing gear', function()
        local removed = {}
        local player = {
            getMod = function() return 1 end,
        }
        local effect = function(id, flags)
            return {
                getEffectType = function() return id end,
                getEffectFlags = function() return flags end,
            }
        end
        local wyvern = {
            delStatusEffect = function(_, id) table.insert(removed, id) end,
            getStatusEffects = function()
                return {
                    effect(10, xi.effectFlag.ERASABLE),
                    effect(xi.effect.BLINDNESS, 0),
                    effect(99, 0),
                }
            end,
        }
        local random = math.random
        math.random = function() return 1 end
        xi.job_utils.dragoon.checkForRemovableEffectsOnSpiritLink(player, wyvern)
        math.random = random

        assert(#removed == 16)
        assert(removed[15] == 10 and removed[16] == xi.effect.BLINDNESS)
    end)
end)
