require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight one-hour recast checks', function()
    it('rewrites Blood Weapon and Soul Enslavement recasts then returns success', function()
        local player = {
            getMod = function(_, mod)
                assert(mod == xi.mod.ONE_HOUR_RECAST)
                return 15
            end,
        }
        local bloodWeaponRecast
        local bloodWeapon = {
            getRecast = function() return 3600 end,
            setRecast = function(_, value) bloodWeaponRecast = value end,
        }
        local soulEnslavementRecast
        local soulEnslavement = {
            getRecast = function() return 60 end,
            setRecast = function(_, value) soulEnslavementRecast = value end,
        }

        local msg, param = xi.job_utils.dark_knight.checkBloodWeapon(player, {}, bloodWeapon)
        assert(msg == 0 and param == 0 and bloodWeaponRecast == 2700)

        msg, param = xi.job_utils.dark_knight.checkSoulEnslavement(player, {}, soulEnslavement)
        assert(msg == 0 and param == 0 and soulEnslavementRecast == 0)
    end)
end)
