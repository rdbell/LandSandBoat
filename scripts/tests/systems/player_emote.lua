require('scripts/globals/player')

describe('Player emote Full Speed Ahead gate', function()
    it('forwards Cheer only while Full Speed Ahead is active', function()
        local calls = 0
        local active = true
        xi.fsa.onCheer = function() calls = calls + 1 end

        local player = {
            hasStatusEffect = function(_, effect)
                return active and effect == xi.effect.FULL_SPEED_AHEAD
            end,
        }

        xi.player.onPlayerEmote(player, xi.emote.CHEER)
        assert(calls == 1)

        active = false
        xi.player.onPlayerEmote(player, xi.emote.CHEER)
        assert(calls == 1)

        active = true
        xi.player.onPlayerEmote(player, xi.emote.WAVE)
        assert(calls == 1)
    end)
end)
