require('scripts/globals/caskets')

describe('Treasure Casket hint finish routing', function()
    local function chest(vars, calls)
        return {
            getLocalVar = function(_, name) return vars[name] end,
            setLocalVar = function(_, name, value)
                vars[name] = value
                calls.set[#calls.set + 1] = { name, value }
            end,
            setAnimationSub = function(_, ...) calls.animation = { ... } end,
            setStatus = function(_, status) calls.status = status end,
            resetLocalVars = function() calls.reset = true end,
        }
    end

    local function playerFor(casket, messages, broadcasts)
        local player
        player = {
            getZoneID = function() return 999 end,
            getEventTarget = function() return casket end,
            getAlliance = function() return { player } end,
            messageSpecial = function(_, ...) messages[#messages + 1] = { ... } end,
            messageName = function(_, ...) broadcasts[#broadcasts + 1] = { ... } end,
        }
        return player
    end

    it('digit-triple hint increments attempts and removes its hint', function()
        local vars = {
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]CORRECT_NUM'] = 42,
            ['[caskets]ATTEMPTS'] = 3,
            ['[caskets]FAILED_ATEMPTS'] = 0,
            ['[caskets]HINTS_TABLE'] = 3,
        }
        local calls, messages, broadcasts = { set = {} }, {}, {}
        local oldZone, oldRandom = zones[999], math.random
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        math.random = function(count) assert(count == 1); return 1 end
        local casket = chest(vars, calls)

        xi.caskets.onEventFinish(playerFor(casket, messages, broadcasts), 0, 258, nil)

        math.random, zones[999] = oldRandom, oldZone
        assert(messages[1][1] == 1018 and messages[1][2] == 4 and messages[1][3] == 5 and messages[1][4] == 6)
        assert(vars['[caskets]FAILED_ATEMPTS'] == 1 and vars['[caskets]HINTS_TABLE'] == 0)
        assert(#broadcasts == 0)
    end)

    it('range hint uses both random bounds', function()
        local vars = {
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]CORRECT_NUM'] = 45,
            ['[caskets]ATTEMPTS'] = 3,
            ['[caskets]FAILED_ATEMPTS'] = 0,
            ['[caskets]HINTS_TABLE'] = 7,
        }
        local calls, messages, broadcasts = { set = {} }, {}, {}
        local oldZone, oldRandom = zones[999], math.random
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        local rolls = { 1, 2, 4 }
        math.random = function(low, high)
            if high == nil then
                assert(low == 1)
            else
                assert(low == 1 and high == 9)
            end
            return table.remove(rolls, 1)
        end
        local casket = chest(vars, calls)

        xi.caskets.onEventFinish(playerFor(casket, messages, broadcasts), 0, 258, nil)

        math.random, zones[999] = oldRandom, oldZone
        assert(messages[1][1] == 1013 and messages[1][2] == 32 and messages[1][3] == 54)
        assert(vars['[caskets]FAILED_ATEMPTS'] == 1 and vars['[caskets]HINTS_TABLE'] == 0)
        assert(#broadcasts == 0)
    end)

    it('last parity hint reveals the answer, broadcasts failure, and removes the casket', function()
        local vars = {
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]CORRECT_NUM'] = 42,
            ['[caskets]ATTEMPTS'] = 3,
            ['[caskets]FAILED_ATEMPTS'] = 2,
            ['[caskets]HINTS_TABLE'] = 1,
        }
        local calls, messages, broadcasts = { set = {} }, {}, {}
        local oldZone, oldRandom = zones[999], math.random
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        math.random = function(count) assert(count == 1); return 1 end
        local casket = chest(vars, calls)

        xi.caskets.onEventFinish(playerFor(casket, messages, broadcasts), 0, 258, nil)

        math.random, zones[999] = oldRandom, oldZone
        assert(messages[1][1] == 1012 and messages[1][2] == 0)
        assert(messages[2][1] == 1009 and messages[2][2] == 42)
        assert(broadcasts[1][1] == 1008 and calls.animation[1] == 0 and calls.status == xi.status.DISAPPEAR and calls.reset)
    end)

    it('with no available hint it only reports failure', function()
        local vars = {
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]CORRECT_NUM'] = 42,
            ['[caskets]ATTEMPTS'] = 3,
            ['[caskets]FAILED_ATEMPTS'] = 0,
            ['[caskets]HINTS_TABLE'] = 0,
        }
        local calls, messages, broadcasts = { set = {} }, {}, {}
        local oldZone, oldRandom = zones[999], math.random
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        math.random = function(count) assert(count == 0); return 1 end
        local casket = chest(vars, calls)

        xi.caskets.onEventFinish(playerFor(casket, messages, broadcasts), 0, 258, nil)

        math.random, zones[999] = oldRandom, oldZone
        assert(messages[1][1] == 1019 and #calls.set == 0 and #broadcasts == 0)
    end)
end)
