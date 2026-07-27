require('scripts/globals/job_utils/rune_fencer')

describe('Rune Fencer Swipe/Lunge', function()
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

    it('uses only the newest rune for Swipe when other runes are active', function()
        local removed = {}
        local info
        local mods = { [xi.mod.UTSUSEMI] = 1 }
        local runes = { xi.effect.IGNIS, xi.effect.GELUS }
        local target = {
            getID = function() return 9 end,
            updateClaim = function() end,
            getMod = function(_, mod) return mods[mod] or 0 end,
            setMod = function(_, mod, value) mods[mod] = value end,
            getStatusEffect = function() return nil end,
            delStatusEffect = function() end,
        }
        local player = {
            getNewestRuneEffect = function() return runes[#runes] end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            countEffect = function() return 1 end,
            getWeaponSkillType = function() return xi.skill.SWORD end,
            getMod = function() return 0 end,
            getJobPointLevel = function() return 0 end,
            getMerit = function() return 0 end,
            getSkillLevel = function() return 0 end,
            getILvlSkill = function() return 0 end,
            removeNewestRune = function()
                table.insert(removed, table.remove(runes))
            end,
        }
        local ability = { getID = function() return xi.jobAbility.SWIPE end, setMsg = function() end }
        local action = {
            info = function(_, _, value) info = value end,
            resolution = function() end,
            setAnimation = function() error('fully shadowed Swipe must not animate') end,
        }

        assert(xi.job_utils.rune_fencer.useSwipeLunge(player, target, ability, action) == 1)
        assert(#removed == 1 and removed[1] == xi.effect.GELUS)
        assert(#runes == 1 and runes[1] == xi.effect.IGNIS)
        assert(info == 4)
    end)

    it('uses every active rune for Lunge in newest-first order', function()
        local removed = {}
        local info
        local message
        local mods = { [xi.mod.UTSUSEMI] = 3 }
        local runes = { xi.effect.IGNIS, xi.effect.IGNIS, xi.effect.GELUS }
        local target = {
            getID = function() return 9 end,
            updateClaim = function() end,
            getMod = function(_, mod) return mods[mod] or 0 end,
            setMod = function(_, mod, value) mods[mod] = value end,
            getStatusEffect = function()
                return { setIcon = function() end }
            end,
            delStatusEffect = function() end,
        }
        local player = {
            getNewestRuneEffect = function() return runes[#runes] end,
            getHighestRuneEffect = function() return xi.effect.IGNIS end,
            countEffect = function(_, effect)
                assert(effect == xi.effect.IGNIS)
                return 2
            end,
            getWeaponSkillType = function() return xi.skill.SWORD end,
            getMod = function() return 0 end,
            getJobPointLevel = function() return 0 end,
            getMerit = function() return 0 end,
            getActiveRuneCount = function() return #runes end,
            getSkillLevel = function() return 0 end,
            getILvlSkill = function() return 0 end,
            removeNewestRune = function()
                table.insert(removed, table.remove(runes))
            end,
        }
        local ability = {
            getID = function() return xi.jobAbility.LUNGE end,
            setMsg = function(_, value) message = value end,
        }
        local action = {
            info = function(_, _, value) info = value end,
            resolution = function() end,
            setAnimation = function() error('fully shadowed Lunge must not animate') end,
        }

        assert(xi.job_utils.rune_fencer.useSwipeLunge(player, target, ability, action) == 3)
        assert(#runes == 0)
        assert(removed[1] == xi.effect.GELUS and removed[2] == xi.effect.IGNIS and removed[3] == xi.effect.IGNIS)
        assert(info == 2 and message == xi.msg.basic.SHADOW_ABSORB)
    end)
end)
