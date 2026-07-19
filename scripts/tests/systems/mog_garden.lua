require('scripts/globals/mog_garden')

describe('Mog Garden initialization', function()
    it('hides every NPC before revealing the three default NPCs', function()
        local changes = {}
        local function npc(id)
            return {
                setStatus = function(_, status) table.insert(changes, { id, status }) end,
            }
        end

        local defaults = zones[xi.zone.MOG_GARDEN].npc
        local ok, err = pcall(function()
            xi.mog_garden.onInitialize({
                getNPCs = function()
                    return {
                        npc('other'),
                        npc(defaults.GREEN_THUMB_MOOGLE),
                        npc(defaults.MOG_DINGHY),
                        npc(defaults.PORTER_MOOGLE),
                    }
                end,
            }, function(id)
                return npc(id)
            end)
        end)
        assert(ok, err)

        assert(#changes == 7)
        assert(changes[1][1] == 'other' and changes[1][2] == xi.status.DISAPPEAR)
        assert(changes[2][1] == defaults.GREEN_THUMB_MOOGLE and changes[2][2] == xi.status.DISAPPEAR)
        assert(changes[3][1] == defaults.MOG_DINGHY and changes[3][2] == xi.status.DISAPPEAR)
        assert(changes[4][1] == defaults.PORTER_MOOGLE and changes[4][2] == xi.status.DISAPPEAR)
        assert(changes[5][1] == defaults.GREEN_THUMB_MOOGLE and changes[5][2] == xi.status.NORMAL)
        assert(changes[6][1] == defaults.MOG_DINGHY and changes[6][2] == xi.status.NORMAL)
        assert(changes[7][1] == defaults.PORTER_MOOGLE and changes[7][2] == xi.status.NORMAL)
    end)

    it('does not look up default NPCs when the zone has no NPCs', function()
        local ok, err = pcall(function()
            xi.mog_garden.onInitialize({
                getNPCs = function()
                    return {}
                end,
            }, function()
                error('empty Mog Garden must not look up NPCs')
            end)
        end)
        assert(ok, err)
    end)
end)
