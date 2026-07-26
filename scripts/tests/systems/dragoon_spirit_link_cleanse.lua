require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Link guaranteed cleanse', function()
    it('removes its fixed DoT, Doom, and sleep effects without gear', function()
        local removed = {}
        local player = {
            getMod = function() return 0 end,
        }
        local wyvern = {
            delStatusEffect = function(_, effect) table.insert(removed, effect) end,
        }

        xi.job_utils.dragoon.checkForRemovableEffectsOnSpiritLink(player, wyvern)

        local expected = {
            xi.effect.POISON, xi.effect.BIO, xi.effect.DIA, xi.effect.REQUIEM,
            xi.effect.BURN, xi.effect.FROST, xi.effect.CHOKE, xi.effect.RASP,
            xi.effect.SHOCK, xi.effect.DROWN, xi.effect.DOOM,
            xi.effect.SLEEP_I, xi.effect.SLEEP_II, xi.effect.LULLABY,
        }
        assert(#removed == #expected)
        for i, effect in ipairs(expected) do
            assert(removed[i] == effect)
        end
    end)
end)
