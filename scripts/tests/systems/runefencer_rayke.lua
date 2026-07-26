require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Rayke', function()
    it('applies rune resistance reductions, packs elements, and consumes runes', function()
        local removedRunes = false
        local mods = {}
        local subPower
        local player = {
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            getWeaponSkillType = function() return xi.skill.SWORD end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.MERIT_RAYKE)
                return 3
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.RAYKE_DURATION)
                return 6
            end,
            getAllRuneEffects = function() return { xi.effect.IGNIS, xi.effect.LUX } end,
            removeAllRunes = function() removedRunes = true end,
        }
        local target = {
            getID = function() return 9 end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.RAYKE and params.duration == 36 and params.origin == player)
                return true
            end,
            getStatusEffect = function(_, effect)
                assert(effect == xi.effect.RAYKE)
                return {
                    addMod = function(_, mod, power) mods[mod] = power end,
                    setSubPower = function(_, value) subPower = value end,
                }
            end,
        }
        local action = { info = function(_, id, value) assert(id == 9 and value == 2) end, setAnimation = function() end }

        assert(xi.job_utils.rune_fencer.useRayke(player, target, {}, action) == xi.effect.RAYKE)
        assert(mods[xi.mod.FIRE_RES_RANK] == -1 and mods[xi.mod.LIGHT_RES_RANK] == -1)
        assert(subPower == 113)
        assert(removedRunes)
    end)
end)
