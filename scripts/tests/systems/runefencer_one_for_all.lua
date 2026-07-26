require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer One for All', function()
    it('replaces the effect on every party member when self-targeted', function()
        local calls = {}
        local function member(name)
            return {
                delStatusEffectSilent = function(_, effect)
                    calls[name] = calls[name] or {}
                    calls[name].removed = effect
                end,
                addStatusEffect = function(_, effect, params)
                    calls[name] = calls[name] or {}
                    calls[name].effect = effect
                    calls[name].params = params
                end,
            }
        end

        local player = {
            getID = function() return 9 end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.ONE_FOR_ALL_DURATION)
                return 7
            end,
            getMaxHP = function() return 1000 end,
        }
        player.getParty = function() return { member('player'), member('member') } end

        assert(xi.job_utils.rune_fencer.useOneForAll(player, player, {}, {}) == nil)
        for _, name in ipairs({ 'player', 'member' }) do
            assert(calls[name].removed == xi.effect.ONE_FOR_ALL)
            assert(calls[name].effect == xi.effect.ONE_FOR_ALL)
            assert(calls[name].params.power == 200 and calls[name].params.duration == 37 and calls[name].params.origin == player)
        end
    end)

    it('does not apply effects when target is not the caster', function()
        local partyRequested = false
        local player = {
            getID = function() return 9 end,
            getJobPointLevel = function() return 0 end,
            getMaxHP = function() return 1000 end,
            getParty = function() partyRequested = true end,
        }
        local target = { getID = function() return 10 end }

        assert(xi.job_utils.rune_fencer.useOneForAll(player, target, {}, {}) == nil)
        assert(not partyRequested)
    end)
end)
