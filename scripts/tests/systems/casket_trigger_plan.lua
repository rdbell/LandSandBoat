require('scripts/globals/caskets')

describe('Treasure Casket trigger routing', function()
    local function npc(vars)
        local animation
        local zone = {
            queryEntitiesByName = function()
                return { { getID = function() return 100 end } }
            end,
        }
        return {
            getID = function() return 100 end,
            getZone = function() return zone end,
            getLocalVar = function(_, name) return vars[name] end,
            setLocalVar = function(_, name, value) vars[name] = value end,
            setAnimationSub = function(_, value) animation = value end,
            animationSub = function() return animation end,
        }
    end

    it('starts the locked event with remaining attempts for the owner', function()
        local chest = npc({
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]PARTYID'] = 77,
            ['[caskets]ATTEMPTS'] = 5,
            ['[caskets]FAILED_ATEMPTS'] = 2,
            ['[caskets]ITEMS_SET'] = 1,
        })
        local started
        local player = {
            getLeaderID = function() return 77 end,
            getZoneID = function() return 999 end,
            startEvent = function(_, ...) started = { ... } end,
        }

        xi.caskets.onTrigger(player, chest)

        assert(started[1] == 1002 and started[2] == 3)
        assert(#started == 9 and started[3] == 0 and started[9] == 0)
    end)

    it('opens an unlocked temporary casket and starts its item event', function()
        local chest = npc({
            ['[caskets]LOOT_TYPE'] = 1,
            ['[caskets]LOCKED'] = 0,
            ['[caskets]PARTYID'] = 77,
            ['[caskets]SPAWNSTATUS'] = 1,
            ['[caskets]ATTEMPTS'] = 0,
            ['[caskets]FAILED_ATEMPTS'] = 0,
            ['[caskets]TEMP1'] = 11,
            ['[caskets]TEMP2'] = 22,
            ['[caskets]TEMP3'] = 33,
            ['[caskets]ITEMS_SET'] = 1,
        })
        local started
        local record
        local oldRecord = xi.roe.onRecordTrigger
        xi.roe.onRecordTrigger = function(_, id) record = id end
        local player = {
            getLeaderID = function() return 77 end,
            getZoneID = function() return 999 end,
            getEminenceProgress = function(_, id) return id == 4019 end,
            startEvent = function(_, ...) started = { ... } end,
        }

        xi.caskets.onTrigger(player, chest)
        xi.roe.onRecordTrigger = oldRecord

        assert(chest.animationSub() == 1 and chest:getLocalVar('[caskets]SPAWNSTATUS') == 2)
        assert(record == 4019)
        assert(started[1] == 1001 and started[2] == 11 and started[3] == 22 and started[4] == 33)
        assert(#started == 9 and started[5] == 0 and started[9] == 0)
    end)

    it('does nothing when the player is not the chest owner', function()
        local chest = npc({
            ['[caskets]LOOT_TYPE'] = 2,
            ['[caskets]LOCKED'] = 1,
            ['[caskets]PARTYID'] = 77,
            ['[caskets]ATTEMPTS'] = 5,
            ['[caskets]FAILED_ATEMPTS'] = 0,
            ['[caskets]ITEMS_SET'] = 1,
        })
        local starts = 0
        local player = {
            getLeaderID = function() return 66 end,
            getZoneID = function() return 999 end,
            startEvent = function() starts = starts + 1 end,
        }

        xi.caskets.onTrigger(player, chest)

        assert(starts == 0 and chest.animationSub() == nil)
    end)
end)
