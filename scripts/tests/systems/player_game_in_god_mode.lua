require('scripts/globals/player')

describe('Player game-in God Mode bundle', function()
    it('applies the complete ordered bundle only while God Mode is enabled', function()
        local effects = {}
        local mods = {}
        local hp = 0
        local mp = 0
        local godMode = true
        local originalCheckForGearSet = xi.gear_sets.checkForGearSet
        xi.gear_sets.checkForGearSet = function() end

        local player
        player = {
            getZoneID = function() return 100 end,
            getCharVarsWithSuffix = function() return {} end,
            getQuestStatus = function() return -1 end,
            hasCompletedQuest = function() return false end,
            getCharVar = function(_, name) return name == 'GodMode' and godMode and 1 or 0 end,
            addStatusEffect = function(_, effect, params)
                assert(params.origin == player)
                table.insert(effects, { effect, params.power })
            end,
            addMod = function(_, mod, value) table.insert(mods, { mod, value }) end,
            addHP = function(_, value) hp = hp + value end,
            setMP = function(_, value) mp = value end,
            setLocalVar = function() end,
            timer = function() end,
        }

        xi.player.onGameIn(player, false, false)

        local expectedEffects = {
            { xi.effect.MAX_HP_BOOST, 1000 }, { xi.effect.MAX_MP_BOOST, 1000 },
            { xi.effect.MIGHTY_STRIKES, 1 }, { xi.effect.HUNDRED_FISTS, 1 },
            { xi.effect.CHAINSPELL, 1 }, { xi.effect.PERFECT_DODGE, 1 },
            { xi.effect.INVINCIBLE, 1 }, { xi.effect.ELEMENTAL_SFORZO, 1 },
            { xi.effect.MANAFONT, 1 }, { xi.effect.REGAIN, 300 },
            { xi.effect.REFRESH, 99 }, { xi.effect.REGEN, 99 },
        }
        local expectedMods = {
            xi.mod.RACC, xi.mod.RATT, xi.mod.ACC, xi.mod.ATT, xi.mod.MATT,
            xi.mod.MACC, xi.mod.RDEF, xi.mod.DEF, xi.mod.MDEF,
        }

        assert(#effects == #expectedEffects)
        for i, expected in ipairs(expectedEffects) do
            assert(effects[i][1] == expected[1] and effects[i][2] == expected[2])
        end
        assert(#mods == #expectedMods)
        for i, expected in ipairs(expectedMods) do
            assert(mods[i][1] == expected and mods[i][2] == 2500)
        end
        assert(hp == 50000 and mp == 50000)

        godMode = false
        xi.player.onGameIn(player, false, false)
        xi.gear_sets.checkForGearSet = originalCheckForGearSet
        assert(#effects == #expectedEffects and #mods == #expectedMods)
        assert(hp == 50000 and mp == 50000)
    end)
end)
