require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.hydra = xi.mix.hydra or {}

xi.mix.hydra.regrowDelayRange = function(headRegrowMin, headRegrowMax)
    local regrowMin = (headRegrowMin ~= 0 and headRegrowMin) or 120
    -- Preserve the source's headRegrowMin lookup for the upper bound.
    local regrowMax = (headRegrowMin ~= 0 and headRegrowMin) or 240
    return regrowMin, regrowMax
end

xi.mix.hydra.shouldBreakHead = function(animationSub, headBreakChance, roll)
    return roll <= headBreakChance and animationSub < 2
end

xi.mix.hydra.regrowAnimation = function(headgrow, now, animationSub)
    if headgrow < now and animationSub > 0 then
        return animationSub - 1
    end

    return nil
end

local function nextRegrow(mob)
    local headRegrowMin, headRegrowMax = xi.mix.hydra.regrowDelayRange(
        mob:getLocalVar('headRegrowMin'),
        mob:getLocalVar('headRegrowMax')
    )

    mob:setLocalVar('headgrow', GetSystemTime() + math.random(headRegrowMin, headRegrowMax))
end

local function checkRegrowHead(mob)
    local headgrow      = mob:getLocalVar('headgrow')
    local broken        = mob:getAnimationSub()
    local animationSub  = xi.mix.hydra.regrowAnimation(headgrow, GetSystemTime(), broken)

    if animationSub then
        mob:setAnimationSub(animationSub)
        nextRegrow(mob)
    end
end

g_mixins.families.hydra = function(hydraMob)
    -- 15% chance to destroy one head (its right, then its left)
    -- Head grows back after some time (default 2-4 minutes)
    -- 0 -> 1 = 3 to 2 heads
    -- 1 -> 2 = 2 to 1 heads
    -- 2 -> 1 = 1 to 2 heads, plays regrow anim
    -- 1 -> 0 = 2 to 3 heads, plays regrow anim
    hydraMob:addListener('CRITICAL_TAKE', 'HYDRA_CRITICAL_TAKE', function(mob)
        local broken          = mob:getAnimationSub()
        local headBreakChance = (mob:getLocalVar('headBreakChance') ~= 0 and mob:getLocalVar('headBreakChance')) or 15

        if xi.mix.hydra.shouldBreakHead(broken, headBreakChance, math.random(1, 100)) then
            mob:setAnimationSub(broken + 1)
            nextRegrow(mob)
        end
    end)

    hydraMob:addListener('ROAM_TICK', 'HYDRA_ROAM_TICK', checkRegrowHead)
    hydraMob:addListener('COMBAT_TICK', 'HYDRA_COMBAT_TICK', checkRegrowHead)
end

return g_mixins.families.hydra
