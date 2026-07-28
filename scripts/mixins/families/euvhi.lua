-----------------------------------
--- Euvhi Family Mixin
--  https://ffxiclopedia.fandom.com/wiki/Category:Euvhi
--  https://www.bg-wiki.com/ffxi/Category:Euvhi
--  Euvhi open mouth after 80 seconds, and deal 1.5% base damage
--  Close mouth after taking 350 damage
-----------------------------------
require('scripts/globals/mixins')
-----------------------------------

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.euvhi = xi.mix.euvhi or {}

xi.mix.euvhi.isMouthOpen = function(animationSub)
    return bit.band(animationSub, 0x02) == 0x02
end

xi.mix.euvhi.openMouthPlan = function(mainLvl)
    return { baseDamageModifier = 1 + mainLvl / 2, animationSub = 2, waitMs = 2000 }
end

xi.mix.euvhi.closeMouthPlan = function(now)
    return { baseDamageModifier = 0, changeTime = now + 80, closeMouth = 0, animationSub = 1, waitMs = 2000 }
end

xi.mix.euvhi.combatPlan = function(animationSub, changeTime, closeMouth, now, busy)
    if not xi.mix.euvhi.isMouthOpen(animationSub) and now > changeTime and not busy then
        return 'open'
    elseif xi.mix.euvhi.isMouthOpen(animationSub) and closeMouth == 1 and not busy then
        return 'close'
    end
    return nil
end

xi.mix.euvhi.shouldCloseMouth = function(animationSub, damage)
    return xi.mix.euvhi.isMouthOpen(animationSub) and damage >= 350
end

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

local function isMouthOpen(mob)
    return xi.mix.euvhi.isMouthOpen(mob:getAnimationSub())
end

local function openMouth(mob)
    local plan = xi.mix.euvhi.openMouthPlan(mob:getMainLvl())
    mob:setMobMod(xi.mobMod.BASE_DAMAGE_MODIFIER, plan.baseDamageModifier)
    mob:setAnimationSub(plan.animationSub) -- TODO: the db.. mobs start with animsub 4 or 6? shouldn't even be possible
    mob:wait(plan.waitMs)
end

local function closeMouth(mob)
    local plan = xi.mix.euvhi.closeMouthPlan(GetSystemTime())
    mob:setMobMod(xi.mobMod.BASE_DAMAGE_MODIFIER, plan.baseDamageModifier)
    mob:setLocalVar('[euvhi]changeTime', plan.changeTime)
    mob:setLocalVar('closeMouth', plan.closeMouth)
    mob:setAnimationSub(plan.animationSub) -- TODO: db shows animsub 5?
    mob:wait(plan.waitMs)
end

local function setAggressiveness(mob)
    if isMouthOpen(mob) then
        mob:setAggressive(true)
    else
        mob:setAggressive(false)
    end
end

g_mixins.families.euvhi = function(euvhiMob)
    euvhiMob:addListener('SPAWN', 'EUVHI_SPAWN', function(mob)
        mob:setLocalVar('defaultAnimation', mob:getAnimationSub())
        setAggressiveness(mob)
    end)

    euvhiMob:addListener('ENGAGE', 'EUVHI_ENGAGE', function(mob, target)
        mob:setLocalVar('[euvhi]changeTime', GetSystemTime() + 80)
    end)

    euvhiMob:addListener('COMBAT_TICK', 'EUVHI_CTICK', function(mob)
        local plan = xi.mix.euvhi.combatPlan(mob:getAnimationSub(), mob:getLocalVar('[euvhi]changeTime'), mob:getLocalVar('closeMouth'), GetSystemTime(), xi.combat.behavior.isEntityBusy(mob))
        if plan == 'open' then
            openMouth(mob)
        elseif plan == 'close' then
            closeMouth(mob)
        end
    end)

    euvhiMob:addListener('TAKE_DAMAGE', 'EUVHI_TAKE_DAMAGE', function(mob, damage, attacker, attackType, damageType)
        if xi.mix.euvhi.shouldCloseMouth(mob:getAnimationSub(), damage) then
            mob:setLocalVar('closeMouth', 1)
        end
    end)

    euvhiMob:addListener('ROAM_TICK', 'EUVHI_RTICK', function(mob)
        setAggressiveness(mob)
    end)

    euvhiMob:addListener('DISENGAGE', 'EUVHI_DISENGAGE', function(mob)
        mob:setLocalVar('[euvhi]changeTime', 0)
        mob:setAnimationSub(mob:getLocalVar('defaultAnimation'))
        setAggressiveness(mob)
    end)
end

return g_mixins.families.euvhi
