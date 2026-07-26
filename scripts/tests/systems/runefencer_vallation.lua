require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Vallation', function()
    it('replaces Valiance and applies rune SDT mods with Inspiration Fast Cast', function()
        local deleted
        local applied = {}
        local sdtMods = {}
        local player = {
            getID = function() return 7 end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getAllRuneEffects = function() return { xi.effect.IGNIS, xi.effect.LUX } end,
            getMerit = function(_, merit)
                if merit == xi.merit.MERIT_VALLATION_EFFECT then return 5 end
                if merit == xi.merit.MERIT_INSPIRATION then return 10 end
                return 0
            end,
            getMod = function(_, mod)
                if mod == xi.mod.ENHANCES_INSPIRATION then return 2 end
                return 0
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.VALLATION_DURATION)
                return 30
            end,
            hasStatusEffect = function() return false end,
            delStatusEffectSilent = function(_, effect) deleted = effect end,
            addStatusEffect = function(_, effect, params)
                applied[effect] = params
                return effect == xi.effect.VALLATION
            end,
            getStatusEffect = function(_, effect)
                assert(effect == xi.effect.VALLATION)
                return { addMod = function(_, mod, power) sdtMods[mod] = power end }
            end,
        }
        local ability = { getID = function() return xi.jobAbility.VALLATION end }
        local action = { info = function(_, id, value) assert(id == 7 and value == 1) end }

        assert(xi.job_utils.rune_fencer.useVallationValiance(player, player, ability, action) == xi.effect.VALLATION)
        assert(deleted == xi.effect.VALIANCE)
        assert(applied[xi.effect.VALLATION].power == 2000 and applied[xi.effect.VALLATION].duration == 150)
        assert(sdtMods[xi.mod.ICE_SDT] == -2000 and sdtMods[xi.mod.DARK_SDT] == -2000)
        assert(applied[xi.effect.FAST_CAST].power == 12 and applied[xi.effect.FAST_CAST].duration == 150)
    end)
end)
