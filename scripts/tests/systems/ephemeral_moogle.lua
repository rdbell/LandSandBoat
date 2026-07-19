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
