--[[
https://ffxiclopedia.fandom.com/wiki/Category:Hpemde
https://www.bg-wiki.com/ffxi/Category:Hpemde
--]]
require('scripts/globals/mixins')
-----------------------------------

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.hpemde = xi.mix.hpemde or {}

xi.mix.hpemde.divePlan = function(canDive)
    return { autoAttack = false, mobAbility = false, hideName = canDive, untargetable = canDive, animationSub = canDive and 5 or nil }
end

xi.mix.hpemde.surfacePlan = function(animationSub)
    return { hideName = false, untargetable = false, animationSub = (animationSub == 0 or animationSub == 5) and 6 or nil, waitMs = (animationSub == 0 or animationSub == 5) and 2000 or 0 }
end

xi.mix.hpemde.openMouthPlan = function(mainLvl)
    return { baseDamageModifier = mainLvl + 2, damageMod = 10000, animationSub = 3, waitMs = 2000 }
end

xi.mix.hpemde.closeMouthPlan = function(battleTime)
    return { baseDamageModifier = 0, damageMod = 0, changeTime = battleTime + 30, animationSub = 6, waitMs = 2000 }
end

xi.mix.hpemde.combatPlan = function(damaged, hp, maxHP, disengageTime, battleTime, animationSub, changeTime, closeMouthHP)
    if damaged == 0 then
        if hp < maxHP then
            return { action = 'activate', changeTime = battleTime + 30 }
        elseif disengageTime > 0 and battleTime > disengageTime then
            return { action = 'disengage' }
        end
    elseif animationSub == 6 and battleTime > changeTime then
        return { action = 'open' }
    elseif animationSub == 3 and hp < closeMouthHP then
        return { action = 'close' }
    end

    return nil
end

xi.mix.hpemde.shouldCloseMouth = function(animationSub, damage)
    return animationSub == 3 and damage >= 250
end

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

local function dive(mob)
    local plan = xi.mix.hpemde.divePlan(mob:getPool() ~= xi.mobPool.HPEMDE_NO_DIVING)
    mob:setAutoAttackEnabled(plan.autoAttack)
    mob:setMobAbilityEnabled(plan.mobAbility)

    -- Om'hpedme in north half of Al'Taieu do not dive or become untargetable
    if plan.animationSub then
        mob:hideName(plan.hideName)
        mob:setUntargetable(plan.untargetable)
        mob:setAnimationSub(plan.animationSub)
    end
end

local function surface(mob)
    local plan = xi.mix.hpemde.surfacePlan(mob:getAnimationSub())
    mob:hideName(plan.hideName)
    mob:setUntargetable(plan.untargetable)
    if plan.animationSub then
        mob:setAnimationSub(plan.animationSub)
        mob:wait(plan.waitMs)
    end
end

-- Hpemde take 100% increased damage and deal 2x base damage in open mouth form
local function openMouth(mob)
    local plan = xi.mix.hpemde.openMouthPlan(mob:getMainLvl())
    mob:setMobMod(xi.mobMod.BASE_DAMAGE_MODIFIER, plan.baseDamageModifier)
    mob:setMod(xi.mod.DMG, plan.damageMod)
    mob:setAnimationSub(plan.animationSub)
    mob:wait(plan.waitMs)
end

local function closeMouth(mob)
    local plan = xi.mix.hpemde.closeMouthPlan(mob:getBattleTime())
    mob:setMobMod(xi.mobMod.BASE_DAMAGE_MODIFIER, plan.baseDamageModifier)
    mob:setMod(xi.mod.DMG, plan.damageMod)
    mob:setLocalVar('[hpemde]changeTime', plan.changeTime)
    mob:setAnimationSub(plan.animationSub)
    mob:wait(plan.waitMs)
end

g_mixins.families.hpemde = function(hpemdeMob)
    hpemdeMob:addListener('SPAWN', 'HPEMDE_SPAWN', function(mob)
        mob:setMod(xi.mod.REGEN, 10)
        dive(mob)
    end)

    hpemdeMob:addListener('ROAM_TICK', 'HPEMDE_RTICK', function(mob)
        if mob:getHPP() == 100 then
            mob:setLocalVar('[hpemde]damaged', 0)
        end

        if
            mob:getPool() ~= xi.mobPool.HPEMDE_NO_DIVING and
            mob:getAnimationSub() ~= 5
        then
            dive(mob)
        end
    end)

    hpemdeMob:addListener('ENGAGE', 'HPEMDE_ENGAGE', function(mob, target)
        mob:setLocalVar('[hpemde]disengageTime',  mob:getBattleTime() + 45)
        surface(mob)
    end)

    hpemdeMob:addListener('MAGIC_TAKE', 'HPEMDE_MAGIC_TAKE', function(target, caster, spell)
        target:setLocalVar('[hpemde]disengageTime',  target:getBattleTime() + 45)
    end)

    hpemdeMob:addListener('COMBAT_TICK', 'HPEMDE_CTICK', function(mob)
        local plan = xi.mix.hpemde.combatPlan(mob:getLocalVar('[hpemde]damaged'), mob:getHP(), mob:getMaxHP(), mob:getLocalVar('[hpemde]disengageTime'), mob:getBattleTime(), mob:getAnimationSub(), mob:getLocalVar('[hpemde]changeTime'), mob:getLocalVar('[hpemde]closeMouthHP'))
        if plan then
            if plan.action == 'activate' then
                mob:setAutoAttackEnabled(true)
                mob:setMobAbilityEnabled(true)
                mob:setLocalVar('[hpemde]damaged', 1)
                mob:setLocalVar('[hpemde]changeTime', plan.changeTime)
            elseif plan.action == 'disengage' then
                mob:setLocalVar('[hpemde]disengageTime', 0)
                mob:disengage()
            elseif plan.action == 'open' then
                openMouth(mob)
            elseif plan.action == 'close' then
                closeMouth(mob)
            end
        end
    end)

    hpemdeMob:addListener('TAKE_DAMAGE', 'HPEMDE_TAKE_DAMAGE', function(mob, damage, attacker, attackType, damageType)
        if xi.mix.hpemde.shouldCloseMouth(mob:getAnimationSub(), damage) then
            closeMouth(mob)
        end
    end)
end

return g_mixins.families.hpemde
