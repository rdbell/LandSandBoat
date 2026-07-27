require('scripts/globals/abyssea')

describe('Abyssea NM drop routing', function()
    it('gives normal drops to the claimant and atma to eligible alliance members', function()
        local claimantHeld = {}
        local allyHeld = {}
        local claimant = {
            getAlliance = function() return {} end,
            hasKeyItem = function(_, keyItem) return claimantHeld[keyItem] or false end,
        }
        local ally = {
            hasKeyItem = function(_, keyItem) return allyHeld[keyItem] or false end,
        }
        claimant.getAlliance = function() return { claimant, ally } end

        stub('GetPlayerByID', function(id) return id == 42 and claimant or nil end)

        local grants = {}
        local oldGiveKeyItem = npcUtil.giveKeyItem
        npcUtil.giveKeyItem = function(player, keyItem)
            grants[#grants + 1] = { player, keyItem }
            if player == claimant then
                claimantHeld[keyItem] = true
            else
                allyHeld[keyItem] = true
            end
        end

        local mob = {
            getName = function() return 'Alkonost' end,
            getLocalVar = function(_, name)
                if name == '[ClaimedBy]' then return 42 end
                if name == '[AbysseaRedProc]' then return 1 end
                return 0
            end,
        }
        xi.abyssea.giveNMDrops(mob, nil, { text = { PLAYER_KEYITEM_OBTAINED = 0 } })
        npcUtil.giveKeyItem = oldGiveKeyItem

        assert(#grants == 3)
        assert(grants[1][1] == claimant and grants[1][2] == xi.ki.TATTERED_HIPPOGRYPH_WING)
        assert(grants[2][1] == claimant and grants[2][2] == xi.ki.ATMA_OF_GALES)
        assert(grants[3][1] == ally and grants[3][2] == xi.ki.ATMA_OF_GALES)
    end)

    it('does not grant catalog drops without a claimant', function()
        stub('GetPlayerByID', function() return nil end)
        local grants = 0
        local oldGiveKeyItem = npcUtil.giveKeyItem
        npcUtil.giveKeyItem = function() grants = grants + 1 end

        local mob = {
            getName = function() return 'Alkonost' end,
            getLocalVar = function(_, name)
                if name == '[ClaimedBy]' then return 42 end
                return 1
            end,
        }
        xi.abyssea.giveNMDrops(mob, nil, { text = { PLAYER_KEYITEM_OBTAINED = 0 } })
        npcUtil.giveKeyItem = oldGiveKeyItem

        assert(grants == 0)
    end)
end)
