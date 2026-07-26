require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Liement', function()
    it('replaces ward effects on the caster with packed rune absorb types', function()
        local removed = {}
        local applied
        local player = {
            getID = function() return 9 end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getAllRuneEffects = function() return { xi.effect.IGNIS, xi.effect.IGNIS, xi.effect.LUX } end,
            getMod = function(_, mod)
                if mod == xi.mod.LIEMENT_DURATION then return 5 end
                if mod == xi.mod.LIEMENT_EXTENDS_TO_AREA then return 0 end
                error('unexpected mod')
            end,
            delStatusEffectSilent = function(_, effect) table.insert(removed, effect) end,
            addStatusEffect = function(_, effect, params) applied = { effect = effect, params = params } end,
        }
        local action = { info = function(_, id, value) assert(id == 9 and value == 1) end }

        assert(xi.job_utils.rune_fencer.useLiement(player, player, {}, action) == xi.effect.LIEMENT)
        assert(removed[1] == xi.effect.VALLATION and removed[2] == xi.effect.VALIANCE and removed[3] == xi.effect.LIEMENT)
        assert(applied.effect == xi.effect.LIEMENT)
        assert(applied.params.power == 15 and applied.params.duration == 15 and applied.params.origin == player and applied.params.subPower == 0xD77)
    end)

    it('extends the replacement request to every party member when enabled', function()
        local calls = {}
        local function member(name)
            return {
                delStatusEffectSilent = function(_, effect)
                    calls[name] = calls[name] or { removed = {} }
                    table.insert(calls[name].removed, effect)
                end,
                addStatusEffect = function(_, effect, params)
                    calls[name] = calls[name] or { removed = {} }
                    calls[name].effect, calls[name].params = effect, params
                end,
            }
        end
        local player = member('player')
        player.getID = function() return 9 end
        player.getHighestRuneEffect = function() return xi.effect.TENEBRAE end
        player.getAllRuneEffects = function() return { xi.effect.TENEBRAE } end
        player.getMod = function(_, mod)
            if mod == xi.mod.LIEMENT_DURATION then return 0 end
            if mod == xi.mod.LIEMENT_EXTENDS_TO_AREA then return 1 end
            error('unexpected mod')
        end
        player.getParty = function() return { player, member('member') } end
        local action = { info = function(_, id, value) assert(id == 9 and value == 8) end }

        assert(xi.job_utils.rune_fencer.useLiement(player, player, {}, action) == xi.effect.LIEMENT)
        for _, name in ipairs({ 'player', 'member' }) do
            assert(#calls[name].removed == 3 and calls[name].removed[3] == xi.effect.LIEMENT)
            assert(calls[name].effect == xi.effect.LIEMENT)
            assert(calls[name].params.power == 15 and calls[name].params.duration == 10 and calls[name].params.subPower == 12 and calls[name].params.origin == player)
        end
    end)

    it('does not apply effects when target is not the caster', function()
        local partyRequested = false
        local player = {
            getID = function() return 9 end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getParty = function() partyRequested = true end,
        }
        local target = { getID = function() return 10 end }
        local action = { info = function(_, id, value) assert(id == 10 and value == 1) end }

        assert(xi.job_utils.rune_fencer.useLiement(player, target, {}, action) == nil)
        assert(not partyRequested)
    end)
end)
