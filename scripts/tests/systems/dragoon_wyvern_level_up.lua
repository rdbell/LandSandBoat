require('scripts/globals/job_utils/dragoon')

describe('Dragoon wyvern level-up effects', function()
    it('applies wyvern and master modifiers and records the new experience', function()
        local wyvernMods = {}
        local masterMods = {}
        local localVars = { wyvern_exp = 150, level_Ups = 3 }
        local message
        local healthUpdated = false
        local wyvern = {
            getLocalVar = function(_, key) return localVars[key] end,
            setLocalVar = function(_, key, value) localVars[key] = value end,
            addMod = function(_, mod, power) wyvernMods[mod] = power end,
            updateHealth = function() healthUpdated = true end,
            getMaxHP = function() return 900 end,
            setHP = function(_, hp) assert(hp == 900) end,
        }
        local player = {
            getPet = function() return wyvern end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.WYVERN_ATTR_BONUS)
                return 3
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.WYVERN_ATTRIBUTE_DA)
                return 2
            end,
            addMod = function(_, mod, power) masterMods[mod] = power end,
            messageBasic = function(_, msg, a, b, target) message = { msg, a, b, target } end,
        }

        assert(xi.job_utils.dragoon.addWyvernExp(player, 100) == 1)
        assert(wyvernMods[xi.mod.ACC] == 6)
        assert(wyvernMods[xi.mod.HPP] == 6)
        assert(wyvernMods[xi.mod.ATTP] == 5)
        assert(healthUpdated)
        assert(message[1] == xi.msg.basic.STATUS_INCREASED and message[4] == wyvern)
        assert(masterMods[xi.mod.ATT] == 3 and masterMods[xi.mod.DEF] == 3)
        assert(masterMods[xi.mod.ATTP] == 4 and masterMods[xi.mod.DEFP] == 4)
        assert(masterMods[xi.mod.HASTE_ABILITY] == 200)
        assert(masterMods[xi.mod.DOUBLE_ATTACK] == 2)
        assert(masterMods[xi.mod.ALL_WSDMG_ALL_HITS] == 2)
        assert(localVars.wyvern_exp == 250 and localVars.level_Ups == 4)
    end)
end)
