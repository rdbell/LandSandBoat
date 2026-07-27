require('scripts/globals/job_utils/monk')

describe('Monk Footwork', function()
    it('adds the self effect with weapon damage power and attack-bonus sub-power', function()
        local effect
        local player = {
            getWeaponDmg = function() return 50 end,
            getMod = function(_, mod) assert(mod == xi.mod.FOOTWORK_ATT_BONUS); return 13 end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }

        local result = xi.job_utils.monk.useFootwork(player, {}, {})

        assert(result == xi.effect.FOOTWORK and effect.id == xi.effect.FOOTWORK)
        assert(effect.values.power == 70 and effect.values.subPower == 38 and effect.values.duration == 60 and effect.values.origin == player)
    end)
end)
