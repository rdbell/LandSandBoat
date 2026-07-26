require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Swipe/Lunge terminal outcome', function()
    it('reports a fully shadowed Swipe as a miss, consumes its rune, and skips animation', function()
        local removedRune = false
        local info
        local resolution
        local animation
        local message
        local mods = { [xi.mod.UTSUSEMI] = 1 }
        local target = {
            getID = function() return 9 end,
            updateClaim = function(_, player) assert(player) end,
            getMod = function(_, mod) return mods[mod] or 0 end,
            setMod = function(_, mod, value) mods[mod] = value end,
            getStatusEffect = function() return nil end,
            delStatusEffect = function() end,
        }
        local player = {
            getNewestRuneEffect = function() return xi.effect.IGNIS end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            countEffect = function(_, effect)
                assert(effect == xi.effect.IGNIS)
                return 1
            end,
            getWeaponSkillType = function(_, slot)
                assert(slot == xi.slot.MAIN)
                return xi.skill.SWORD
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.SWIPE)
                return 0
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.SWIPE_EFFECT)
                return 0
            end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.MERIT_RUNE_ENHANCE)
                return 0
            end,
            getSkillLevel = function(_, skill)
                assert(skill == xi.skill.SWORD)
                return 0
            end,
            getILvlSkill = function() return 0 end,
            removeNewestRune = function() removedRune = true end,
        }
        local ability = {
            getID = function() return xi.jobAbility.SWIPE end,
            setMsg = function(_, value) message = value end,
        }
        local action = {
            info = function(_, id, value) info = { id, value } end,
            resolution = function(_, id, value) resolution = { id, value } end,
            setAnimation = function(_, id, value) animation = { id, value } end,
        }

        assert(xi.job_utils.rune_fencer.useSwipeLunge(player, target, ability, action) == 1)
        assert(removedRune and mods[xi.mod.UTSUSEMI] == 0)
        assert(info[1] == 9 and info[2] == 2)
        assert(resolution[1] == 9 and resolution[2] == xi.action.resolution.MISS)
        assert(message == xi.msg.basic.SHADOW_ABSORB)
        assert(animation == nil)
    end)
end)
