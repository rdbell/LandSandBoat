require('scripts/globals/caskets')

describe('Treasure Casket locked finish routing', function()
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

    it('correct input unlocks, opens, broadcasts, and triggers casket RoE', function()
        local vars = {
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]CORRECT_NUM'] = 42,
            ['[caskets]ATTEMPTS'] = 3,
            ['[caskets]FAILED_ATEMPTS'] = 0,
            ['[caskets]HINTS_TABLE'] = 0,
        }
        local calls = { set = {} }
        local oldZone = zones[999]
        local oldRecord = xi.roe.onRecordTrigger
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        local record, broadcast
        local casket = chest(vars, calls)
        local player
        player = {
            getZoneID = function() return 999 end,
            getEventTarget = function() return casket end,
            getAlliance = function() return { player } end,
            messageName = function(_, ...) broadcast = { ... } end,
        }
        xi.roe.onRecordTrigger = function(_, id) record = id end

        xi.caskets.onEventFinish(player, 0, bit.lshift(42, 16) + 1, nil)

        xi.roe.onRecordTrigger = oldRecord
        zones[999] = oldZone
        assert(vars['[caskets]LOCKED'] == 0 and vars['[caskets]SPAWNSTATUS'] == 2)
        assert(calls.animation[1] == 1 and record == 4019)
        assert(broadcast[1] == 1010)
    end)

    it('last wrong input gives the hunch, reveals the answer, broadcasts failure, and removes the casket', function()
        local vars = {
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]CORRECT_NUM'] = 42,
            ['[caskets]ATTEMPTS'] = 3,
            ['[caskets]FAILED_ATEMPTS'] = 2,
            ['[caskets]HINTS_TABLE'] = 0,
        }
        local calls = { set = {} }
        local oldZone = zones[999]
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 1000 } }
        local ownMessages, broadcast = {}, nil
        local casket = chest(vars, calls)
        local player
        player = {
            getZoneID = function() return 999 end,
            getEventTarget = function() return casket end,
            getAlliance = function() return { player } end,
            messageSpecial = function(_, ...) ownMessages[#ownMessages + 1] = { ... } end,
            messageName = function(_, ...) broadcast = { ... } end,
        }

        xi.caskets.onEventFinish(player, 0, bit.lshift(41, 16) + 1, nil)

        zones[999] = oldZone
        assert(calls.set[1][1] == '[caskets]FAILED_ATEMPTS' and calls.set[1][2] == 3)
        assert(ownMessages[1][1] == 1007 and ownMessages[1][2] == 41 and ownMessages[1][3] == 0)
        assert(ownMessages[2][1] == 1009 and ownMessages[2][2] == 42)
        assert(broadcast[1] == 1008 and calls.animation[1] == 0 and calls.status == xi.status.DISAPPEAR and calls.reset)
    end)
end)
