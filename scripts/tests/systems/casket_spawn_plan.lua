require('scripts/globals/caskets')

describe('Treasure Casket spawn initialization', function()
    local function spawn(styleRoll)
        local vars = { ['[caskets]SPAWNTIME'] = 0, ['[caskets]SPAWNSTATUS'] = 0 }
        local model
        local timer
        local npc = {
            getLocalVar = function(_, name) return vars[name] end,
            setLocalVar = function(_, name, value) vars[name] = value end,
            resetLocalVars = function() end,
            setAnimation = function() end,
            setAnimationSub = function() end,
            setPos = function() end,
            setStatus = function() end,
            entityAnimationPacket = function() end,
            setModelId = function(_, value) model = value end,
            timer = function(_, value) timer = value end,
        }
        local zone = {
            queryEntitiesByName = function()
                return { { getID = function() return 100 end } }
            end,
        }
        local mob = {
            getZone = function() return zone end,
            getMainLvl = function() return 42 end,
        }
        local player = {
            getLeaderID = function() return 77 end,
            getZoneID = function() return 999 end,
            getAlliance = function(self) return { self } end,
            hasStatusEffect = function() return false end,
            getStatusEffect = function() return nil end,
            messageSpecial = function() end,
        }

        local oldRandom = math.random
        local oldRate = xi.settings.main.CASKET_DROP_RATE
        local oldZone = zones[999]
        stub('GetNPCByID', function(id) return id == 100 and npc or nil end)
        stub('GetSystemTime', function() return 600000 end)
        math.random = function(low, high)
            if low == nil then return 0 end -- dropChance
            if low == 10 and high == 99 then return 42 end
            if low == 4 and high == 6 then return 5 end
            if low == 1 and high == 100 then return styleRoll end
            error('unexpected random range')
        end
        xi.settings.main.CASKET_DROP_RATE = 1
        zones[999] = { text = { PLAYER_OBTAINS_TEMP_ITEM = 100 } }

        xi.caskets.spawnCasket(player, mob, 1, 2, 3, 4)

        math.random = oldRandom
        xi.settings.main.CASKET_DROP_RATE = oldRate
        zones[999] = oldZone

        return vars, model, timer
    end

    it('initializes a brown locked casket from its resolved rolls', function()
        local vars, model, timer = spawn(15)
        assert(model == 966 and timer == 180000)
        assert(vars['[caskets]PARTYID'] == 77 and vars['[caskets]MOBLVL'] == 42)
        assert(vars['[caskets]ITEMS_SET'] == 0 and vars['[caskets]SPAWNSTATUS'] == 1)
        assert(vars['[caskets]SPAWNTIME'] == 600000)
        assert(vars['[caskets]LOCKED'] == 1 and vars['[caskets]LOOT_TYPE'] == 2)
        assert(vars['[caskets]ATTEMPTS'] == 5 and vars['[caskets]CORRECT_NUM'] == 42)
        assert(vars['[caskets]FAILED_ATEMPTS'] == 0 and vars['[caskets]HINTS_TABLE'] == 1234567)
    end)

    it('initializes a blue temporary-item casket from its resolved rolls', function()
        local vars, model, timer = spawn(16)
        assert(model == 965 and timer == 180000)
        assert(vars['[caskets]PARTYID'] == 77 and vars['[caskets]MOBLVL'] == 42)
        assert(vars['[caskets]ITEMS_SET'] == 0 and vars['[caskets]SPAWNSTATUS'] == 1)
        assert(vars['[caskets]SPAWNTIME'] == 600000)
        assert(vars['[caskets]LOCKED'] == 0 and vars['[caskets]LOOT_TYPE'] == 1)
        assert(vars['[caskets]ATTEMPTS'] == nil and vars['[caskets]CORRECT_NUM'] == nil)
        assert(vars['[caskets]FAILED_ATEMPTS'] == nil and vars['[caskets]HINTS_TABLE'] == nil)
    end)
end)
