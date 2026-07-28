-----------------------------------
-- Mixin for Gigas Beastmaster Notorious Monsters
-- (The kind that either use Familiar or Charm)
-- Currently used by: Enkelados, Eurymedon, Ophion, Pallas - Should also be added to Gigas Beastmaster when Expeditionary Force is implemented.
-----------------------------------
require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.gigasBeastmaster = xi.mix.gigasBeastmaster or {}

local variables =
{
    HPP_TRIGGER           = 'hppTrigger',      -- HP % threshold at which Familiar/Charm may activate
    COOLDOWN              = 'cooldown',        -- Duration in seconds between uses (default 7200)
    ENGAGE_DELAY          = 'engageDelay',     -- Delay after engage before ability can activate (default 2s)
    ENGAGE_READY_TIME     = 'engageReadyTime', -- Timestamp when engage delay expires
    NEXT_READY_TIME       = 'nextReadyTime',   -- Timestamp when cooldown expires
}

local configuration =
{
    minimumHppTrigger   = 40,   -- Minimum use threshold
    maximumHppTrigger   = 60,   -- Maximum use threshold
    cooldownSeconds     = 7200, -- 2 hour delay between uses
    engageDelaySeconds  = 2,    -- Delay after engage
}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.gigasBeastmaster.engagePlan = function(now, delay) return { engageReadyTime = now + delay, nextReadyTime = 0 } end
xi.mix.gigasBeastmaster.abilityPlan = function(now, engageReadyTime, nextReadyTime, hpp, hppTrigger, petAlive, cooldown)
    if now < engageReadyTime or now < nextReadyTime or hpp > hppTrigger then return nil end
    return { ability = petAlive and xi.mobSkill.FAMILIAR_1 or xi.mobSkill.CHARM, nextReadyTime = now + cooldown }
end

g_mixins.families.gigas_beastmaster_nm = function(mob)
    mob:addListener('PRESPAWN', 'GIGAS_BEASTMASTER_PRESPAWN', function(mobArg)
        mobArg:setLocalVar(variables.HPP_TRIGGER,       math.random(configuration.minimumHppTrigger, configuration.maximumHppTrigger))
        mobArg:setLocalVar(variables.COOLDOWN,          configuration.cooldownSeconds)
        mobArg:setLocalVar(variables.ENGAGE_DELAY,      configuration.engageDelaySeconds)
        mobArg:setLocalVar(variables.ENGAGE_READY_TIME, 0)
        mobArg:setLocalVar(variables.NEXT_READY_TIME,   0)
    end)

    mob:addListener('ENGAGE', 'GIGAS_BEASTMASTER_ENGAGE', function(mobArg)
        local delay = mobArg:getLocalVar(variables.ENGAGE_DELAY)

        local plan = xi.mix.gigasBeastmaster.engagePlan(GetSystemTime(), delay)
        mobArg:setLocalVar(variables.ENGAGE_READY_TIME, plan.engageReadyTime)
        mobArg:setLocalVar(variables.NEXT_READY_TIME,   plan.nextReadyTime)
    end)

    mob:addListener('COMBAT_TICK', 'GIGAS_BEASTMASTER_COMBAT_TICK', function(mobArg)
        local currentTime = GetSystemTime()

        local pet = mobArg:getPet()
        local plan = xi.mix.gigasBeastmaster.abilityPlan(currentTime, mobArg:getLocalVar(variables.ENGAGE_READY_TIME), mobArg:getLocalVar(variables.NEXT_READY_TIME), mobArg:getHPP(), mobArg:getLocalVar(variables.HPP_TRIGGER), pet and pet:isAlive(), mobArg:getLocalVar(variables.COOLDOWN))
        if not plan then return end

        mobArg:useMobAbility(plan.ability)

        -- Apply cooldown to the 2-hour timer
        mobArg:setLocalVar(variables.NEXT_READY_TIME, plan.nextReadyTime)
    end)
end

return g_mixins.families.gigas_beastmaster_nm
