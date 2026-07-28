-----------------------------------
-- Wamouracampa family mixin
-- TODO: Halting movement during stance change.
-----------------------------------
require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.wamouracampa = xi.mix.wamouracampa or {}

xi.mix.wamouracampa.canUseEclosion = function(mobId, wamouraOffsets)
    for _, wamouraId in pairs(wamouraOffsets) do
        if mobId + 1 == wamouraId then
            return true
        end
    end
    return false
end

xi.mix.wamouracampa.resetPlan = function(now, delay, hp, maxHP)
    return { formTimeEngaged = now + delay, hitPoints = hp - math.floor(maxHP * 20 / 100) }
end

xi.mix.wamouracampa.shouldStretch = function(animationSub, now, formTimeEngaged, formTimeRoam, hitPoints, hp)
    return animationSub == 5 and now > formTimeEngaged and now > formTimeRoam and hitPoints < hp
end

xi.mix.wamouracampa.shouldCurl = function(now, formTimeEngaged, amount, maxHP, hitPoints, hp)
    return now > formTimeEngaged and (amount > math.floor(maxHP * 5 / 100) or hitPoints > hp)
end

xi.mix.wamouracampa.roamAction = function(animationSub, now, formTimeRoam)
    if now > formTimeRoam then
        if animationSub == 4 then
            return 'curl'
        elseif animationSub == 5 then
            return 'stretch'
        end
    end
    return nil
end

xi.mix.wamouracampa.shouldEclose = function(eclosionTime, now)
    return eclosionTime ~= 0 and now >= eclosionTime
end

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

-- This mobs curl up based on damage taken.
-- If they take a hit higher than 5% oh their Max HP OR if they take a total of 1k (aprox) damage, they will curl.
-- If they are already curled, they will reset conditions and remain curled.
-- They will keep streched so long as none of the above conditions are met. Linked Wamouracampas will obviously stay streched.

local function curlUpRoaming(mob)
    mob:setAnimationSub(5) -- Curl
    mob:setLocalVar('formTimeRoam', GetSystemTime() + math.random(43, 47))
    mob:setMobMod(xi.mobMod.SKILL_LIST, 1162) -- Set Curled Skill List. ('Cannonball' and 'Heat Barrier' only)
    mob:addMod(xi.mod.DMGPHYS, -2500)
    mob:delMod(xi.mod.DMGMAGIC, -2500)
end

local function strechUpRoaming(mob)
    mob:setAnimationSub(4) -- Strech
    mob:setLocalVar('formTimeRoam', GetSystemTime() + math.random(43, 47))
    mob:setMobMod(xi.mobMod.SKILL_LIST, 254) -- Set streched Skill List. (All TP moves except 'Cannonball')
    mob:delMod(xi.mod.DMGPHYS, -2500)
    mob:addMod(xi.mod.DMGMAGIC, -2500)
end

local function curlUpEngaged(mob)
    mob:setAnimationSub(5) -- Curl
    mob:setMobMod(xi.mobMod.SKILL_LIST, 1162) -- Set Curled Skill List. ('Cannonball' and 'Heat Barrier' only)
    mob:addMod(xi.mod.DMGPHYS, -2500)
    mob:delMod(xi.mod.DMGMAGIC, -2500)
end

local function strechUpEngaged(mob)
    mob:setAnimationSub(4) -- Strech
    mob:setMobMod(xi.mobMod.SKILL_LIST, 254) -- Set streched Skill List. (All TP moves except 'Cannonball')
    mob:delMod(xi.mod.DMGPHYS, -2500)
    mob:addMod(xi.mod.DMGMAGIC, -2500)
end

local function resetCount(mob)
    local plan = xi.mix.wamouracampa.resetPlan(GetSystemTime(), math.random(40, 50), mob:getHP(), mob:getMaxHP())
    mob:setLocalVar('formTimeEngaged', plan.formTimeEngaged)
    mob:setLocalVar('hitPoints', plan.hitPoints)
end

g_mixins.families.wamouracampa = function(wamouracampaMob)
    -- Determine if this mob can use eclosion.
    -- Any wamouracampa that is followed by a Wamoura means that it can evolve into it via eclosion.
    local ID = zones[wamouracampaMob:getZoneID()]

    local canUseEclosion = xi.mix.wamouracampa.canUseEclosion(wamouracampaMob:getID(), ID.mob.WAMOURA_OFFSET)

    -- Set spawn.
    wamouracampaMob:addListener('SPAWN', 'WAMOURACAMPA_SPAWN', function(mob)
        mob:setAnimationSub(4)
        mob:setMobMod(xi.mobMod.SKILL_LIST, 254)
        mob:addMod(xi.mod.DMGMAGIC, -2500)
        mob:setLocalVar('hitPoints', mob:getHP())
        mob:setLocalVar('formTimeRoam', GetSystemTime() + math.random(30, 90))
        mob:setLocalVar('formTimeEngaged', GetSystemTime())

        if canUseEclosion then
            mob:setLocalVar('eclosionTime', GetSystemTime() + math.random(2400, 3000))
        end
    end)

    -- Handle regular changes on roam.
    wamouracampaMob:addListener('ROAM_TICK', 'WAMOURACAMPA_ROAM', function(mob)
        local action = xi.mix.wamouracampa.roamAction(mob:getAnimationSub(), GetSystemTime(), mob:getLocalVar('formTimeRoam'))
        if action == 'curl' then
            curlUpRoaming(mob)
        elseif action == 'stretch' then
            strechUpRoaming(mob)
        end

        if canUseEclosion then
            local eclosionTime = mob:getLocalVar('eclosionTime')
            if xi.mix.wamouracampa.shouldEclose(eclosionTime, GetSystemTime()) then
                mob:useMobAbility(xi.mobSkill.ECLOSION, mob)
            end
        end
    end)

    -- First damaging hit makes mob curl if not already.
    wamouracampaMob:addListener('ENGAGE', 'WAMOURACAMPA_ENGAGE', function(mob)
        if
            mob:getLocalVar('hitPoints') < mob:getHP() and
            mob:getAnimationSub() == 4
        then
            curlUpEngaged(mob)
            resetCount(mob)
        end
    end)

    wamouracampaMob:addListener('DISENGAGE', 'WAMOURACAMPA_DISENGAGE', function(mob)
        if canUseEclosion then
            mob:setLocalVar('eclosionTime', GetSystemTime() + math.random(2400, 3000))
        end
    end)

    -- Handle streching from curl.
    wamouracampaMob:addListener('COMBAT_TICK', 'WAMOURACAMPA_COMBAT', function(mob)
        if xi.mix.wamouracampa.shouldStretch(mob:getAnimationSub(), GetSystemTime(), mob:getLocalVar('formTimeEngaged'), mob:getLocalVar('formTimeRoam'), mob:getLocalVar('hitPoints'), mob:getHP()) then
            strechUpEngaged(mob)
            resetCount(mob)
        end
    end)

    -- Handle curling from being streched or remaining curled.
    wamouracampaMob:addListener('TAKE_DAMAGE', 'WAMOURACAMPA_TAKE_DAMAGE', function(mob, amount, attacker, attackType, damageType)
        if xi.mix.wamouracampa.shouldCurl(GetSystemTime(), mob:getLocalVar('formTimeEngaged'), amount, mob:getMaxHP(), mob:getLocalVar('hitPoints'), mob:getHP()) then
            if mob:getAnimationSub() == 4 then
                curlUpEngaged(mob)
            end

            resetCount(mob)
        end
    end)
end

return g_mixins.families.wamouracampa
