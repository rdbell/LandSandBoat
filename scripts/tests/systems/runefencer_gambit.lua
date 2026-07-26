require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Gambit', function()
    it('applies same-element SDT increases and consumes all runes', function()
        local removedRunes = false
        local mods = {}
        local target
        target = {
            getID = function() return 4 end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.GAMBIT and params.power == -1000 and params.duration == 70 and params.origin == target)
                return true
            end,
            getStatusEffect = function(_, effect)
                assert(effect == xi.effect.GAMBIT)
                return { addMod = function(_, mod, power) mods[mod] = power end }
            end,
        }
        local player = {
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getWeaponSkillType = function() return xi.skill.SWORD end,
            getAllRuneEffects = function() return { xi.effect.IGNIS, xi.effect.LUX } end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.GAMBIT_DURATION)
                return 3
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.GAMBIT_DURATION)
                return 7
            end,
            removeAllRunes = function() removedRunes = true end,
        }
        local action = {
            info = function(_, id, value) assert(id == 4 and value == 2) end,
            setAnimation = function(_, id, value) assert(id == 4 and value == 15) end,
        }

        assert(xi.job_utils.rune_fencer.useGambit(player, target, {}, action) == xi.effect.GAMBIT)
        assert(mods[xi.mod.FIRE_SDT] == 1000 and mods[xi.mod.LIGHT_SDT] == 1000)
        assert(removedRunes)
    end)

    it('still consumes runes when the target rejects the effect', function()
        local removedRunes = false
        local target = {
            getID = function() return 4 end,
            addStatusEffect = function() return false end,
            getStatusEffect = function() error('effect must not be read after add failure') end,
        }
        local player = {
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getWeaponSkillType = function() return xi.skill.SWORD end,
            getAllRuneEffects = function() return { xi.effect.IGNIS } end,
            getJobPointLevel = function() return 0 end,
            getMod = function() return 0 end,
            removeAllRunes = function() removedRunes = true end,
        }
        local action = { info = function() end, setAnimation = function() end }

        assert(xi.job_utils.rune_fencer.useGambit(player, target, {}, action) == xi.effect.GAMBIT)
        assert(removedRunes)
    end)
end)
