require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Blood Weapon host', function()
    it('adds the target Blood Weapon request from the actor modifier', function()
        local applied
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.ENHANCES_BLOOD_WEAPON)
                return 20
            end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useBloodWeapon(player, target, {}) == xi.effect.BLOOD_WEAPON)
        assert(applied.effect == xi.effect.BLOOD_WEAPON)
        assert(applied.params.power == 1 and applied.params.duration == 50 and applied.params.origin == player)
    end)
end)
