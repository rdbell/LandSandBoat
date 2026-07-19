require('scripts/globals/hobbies/crafting/ephemeral_moogle')

describe('Ephemeral Moogle trigger', function()
    it('starts its retrieve event with packed stored crystal balances', function()
        local event = nil
        local balances = {
            fire_crystals = 1,
            ice_crystals = 2,
            wind_crystals = 3,
            earth_crystals = 4,
            lightning_crystals = 5,
            water_crystals = 6,
            light_crystals = 7,
            dark_crystals = 8,
        }
        local player = {
            getCurrency = function(_, currency) return balances[currency] end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = {
            getName = function() return 'Ephemeral_Moogle_Wood' end,
        }

        xi.crafting.ephemeralMoogleOnTrigger(player, npc)

        assert(event[1] == 913 and event[2] == 0x00020001 and event[3] == 0x00040003)
        assert(event[4] == 0x00060005 and event[5] == 0x00080007)
        assert(event[6] == 0 and event[7] == 0 and event[8] == 0 and event[9] == 0)
    end)
end)

describe('Ephemeral Moogle trade', function()
    it('confirms accepted crystal items and starts its store event', function()
        local confirmed = {}
        local localVars = {}
        local event = nil
        local balances = {
            fire_crystals = 0,
            ice_crystals = 0,
            wind_crystals = 0,
            earth_crystals = 0,
            lightning_crystals = 0,
            water_crystals = 0,
            light_crystals = 0,
            dark_crystals = 0,
        }
        local player = {
            getCurrency = function(_, currency) return balances[currency] end,
            setLocalVar = function(_, name, value) localVars[name] = value end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local trade = {
            getItemQty = function(_, item)
                if item == xi.item.FIRE_CLUSTER then
                    return 2
                elseif item == xi.item.FIRE_CRYSTAL then
                    return 3
                end

                return 0
            end,
            confirmItem = function(_, item, quantity)
                table.insert(confirmed, { item, quantity })
            end,
        }
        local npc = {
            getName = function() return 'Ephemeral_Moogle_Wood' end,
        }

        xi.crafting.ephemeralMoogleOnTrade(player, npc, trade)

        assert(#confirmed == 2)
        assert(confirmed[1][1] == xi.item.FIRE_CLUSTER and confirmed[1][2] == 2)
        assert(confirmed[2][1] == xi.item.FIRE_CRYSTAL and confirmed[2][2] == 3)
        assert(localVars['[EM]fire_crystals'] == 27)
        assert(event[1] == 915 and event[2] == 0x00030002)
        assert(event[3] == 0 and event[4] == 0 and event[5] == 0)
        assert(event[6] == 0 and event[7] == 0 and event[8] == 0 and event[9] == 0)
    end)
end)
