require('scripts/globals/teleports/survival_guide')

describe('Survival Guide update', function()
    it('adds a favorite, stores the menu, and emits its packed update', function()
        local stored, updated = nil, nil
        local player = {
            getTeleportMenu = function()
                return { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 }
            end,
            setTeleportMenu = function(_, _, favorites)
                stored = { unpack(favorites) }
            end,
            updateEvent = function(_, ...) updated = { ... } end,
        }

        xi.survivalGuide.onEventUpdate(player, 8500, 0x002A0004)

        assert(stored[1] == 42 and stored[2] == 1 and stored[9] == 8 and stored[10] == 0)
        assert(updated[1] == 0x0302012A and updated[2] == 0x07060504 and updated[3] == 8)
    end)
end)

describe('Survival Guide finish', function()
    it('does not teleport when the player is farther than six yalms from the guide', function()
        local player = {
            getZoneID = function() return 999 end,
            checkDistance = function() return 6.1 end,
            hasTeleport = function() return true end,
            setPos = function() error('distant player must not teleport') end,
        }

        xi.survivalGuide.onEventFinish(player, 8500, 1, {})
    end)

    it('uses the valor branch because Lua numeric zero is truthy', function()
        local function player(tutorialBypass, valor, gil)
            local payment, tutorialSet, destination = nil, nil, nil
            local p = {
                getZoneID = function() return 999 end,
                checkDistance = function() return 0 end,
                hasTeleport = function() return true end,
                getCharVar = function() return tutorialBypass end,
                hasKeyItem = function() return false end,
                getCurrency = function() return valor end,
                delCurrency = function(_, currency, amount) payment = { currency, amount } end,
                getGil = function() return gil end,
                delGil = function(_, amount) payment = { 'gil', amount } end,
                setCharVar = function(_, _, value) tutorialSet = value end,
                setPos = function(_, ...) destination = { ... } end,
            }
            return p, function() return payment, tutorialSet, destination end
        end

        local valorPlayer, valorResult = player(0, 50, 0)
        xi.survivalGuide.onEventFinish(valorPlayer, 8500, 0x0101, {})
        local payment, tutorialSet, destination = valorResult()
        assert(payment[1] == 'valor_point' and payment[2] == 50 and tutorialSet == nil and destination[5] == 231)

        local tutorialPlayer, tutorialResult = player(2, 0, 0)
        xi.survivalGuide.onEventFinish(tutorialPlayer, 8500, 0x0201, {})
        payment, tutorialSet, destination = tutorialResult()
        assert(payment == nil and tutorialSet == nil and destination == nil)

        local gilPlayer, gilResult = player(0, 0, 1000)
        xi.survivalGuide.onEventFinish(gilPlayer, 8500, 1, {})
        payment, tutorialSet, destination = gilResult()
        assert(payment == nil and tutorialSet == nil and destination == nil)
    end)
end)
