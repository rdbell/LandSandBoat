-- Note: override random model selection and delay until astral flow via mobmods
--  xi.mobMod.AVATAR_PETID: avatar pet from xi.petId to select model/ability from
--  xi.mobMod.AVATAR_ASTRAL_DELAY: number of milliseconds to delay AF after avatar spawn (defaults to immediately... i.e. value of zero)

require('scripts/globals/mixins')

-- keyed on avatar modelId
local abilityData =
{
    [ 791] = { petId = xi.petId.CARBUNCLE, abilityId =  919 },
    [ 792] = { petId = xi.petId.FENRIR,    abilityId =  839 },
    [ 793] = { petId = xi.petId.IFRIT,     abilityId =  913 },
    [ 794] = { petId = xi.petId.TITAN,     abilityId =  914 },
    [ 795] = { petId = xi.petId.LEVIATHAN, abilityId =  915 },
    [ 796] = { petId = xi.petId.GARUDA,    abilityId =  916 },
    [ 797] = { petId = xi.petId.SHIVA,     abilityId =  917 },
    [ 798] = { petId = xi.petId.RAMUH,     abilityId =  918 },
    [1145] = { petId = xi.petId.DIABOLOS,  abilityId = 1911 },
}

local randomAvatarOptions =
{
    xi.petId.CARBUNCLE,
    xi.petId.FENRIR,
    xi.petId.IFRIT,
    xi.petId.TITAN,
    xi.petId.LEVIATHAN,
    xi.petId.GARUDA,
    xi.petId.SHIVA,
    xi.petId.RAMUH,
}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.avatar = xi.mix.avatar or {}

xi.mix.avatar.astralDelay = function(astralDelayMs)
    return astralDelayMs > 0 and astralDelayMs or 0
end

xi.mix.avatar.spawnPlan = function(petId, randomPetId, astralDelayMs)
    if petId == 0 then
        petId = randomPetId
    end

    for modelId, ability in pairs(abilityData) do
        if ability.petId == petId then
            local delay = xi.mix.avatar.astralDelay(astralDelayMs)
            return { modelId = modelId, astralDelayMs = delay, cleanupDelayMs = delay + 5000 }
        end
    end

    return nil
end

xi.mix.avatar.engagePlan = function(modelId, astralDelayMs)
    local abilityId = abilityData[modelId] and abilityData[modelId].abilityId or 0
    if abilityId > 0 then
        return { abilityId = abilityId, astralDelayMs = xi.mix.avatar.astralDelay(astralDelayMs) }
    end

    return nil
end

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

g_mixins.families.avatar = function(avatarMob)
    avatarMob:addListener('SPAWN', 'AVATAR_SPAWN', function(mob)
        mob:removeListener('AVATAR_MOBSKILL_FINISHED')

        local petId   = mob:getMobMod(xi.mobMod.AVATAR_PETID)
        if petId == 0 then
            -- choose a random petId since it's not specified by the avatar mob
            petId = utils.randomEntry(randomAvatarOptions)
        end

        local plan = xi.mix.avatar.spawnPlan(petId, petId, mob:getMobMod(xi.mobMod.AVATAR_ASTRAL_DELAY))
        if not plan then
            -- if AVATAR_PETID is set and doesn't map to an ability, exit and don't install listeners
            -- this is used for pets that can either use AF or be regular pets (like Pandemonium Lamps)
            return
        end

        mob:setModelId(plan.modelId)
        mob:hideName(false)
        mob:setUntargetable(true)
        mob:setUnkillable(true)
        mob:setAutoAttackEnabled(false)
        mob:setMobAbilityEnabled(false)
        mob:setMagicCastingEnabled(false)

        -- most AF avatars are not actually assigned as a mob pet, avatar is set to engage owner's target in astral_flow.lua
        local master = mob:getMaster()
        if master ~= nil then
            local target = master:getTarget()
            if target ~= nil then
                mob:updateEnmity(target)
            end
        end

        -- avatar dies as soon as AF ability completes
        mob:addListener('WEAPONSKILL_STATE_EXIT', 'AVATAR_MOBSKILL_FINISHED', function(mobArg, skillId, wasExecuted)
            mobArg:setUnkillable(false)
            mobArg:setHP(0)
        end)

        -- If something goes wrong, the avatar will clean itself up 5s after astralDelayMs
        mob:timer(plan.cleanupDelayMs, function(mobArg)
            if mobArg:isAlive() then
                mobArg:setUnkillable(false)
                mobArg:setHP(0)
            end
        end)
    end)

    avatarMob:addListener('ENGAGE', 'AVATAR_ENGAGE', function(mob, target)
        local plan = xi.mix.avatar.engagePlan(mob:getModelId(), mob:getMobMod(xi.mobMod.AVATAR_ASTRAL_DELAY))

        if plan then
            mob:timer(plan.astralDelayMs, function(mobArg)
                if mobArg:isAlive() then
                    mobArg:useMobAbility(plan.abilityId, target)
                end
            end)
        end
    end)
end

return g_mixins.families.avatar
