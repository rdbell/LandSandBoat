-- Aern family mixin
-- Customization:
--   Setting AERN_RERAISE_MAX will determine the number of times it will reraise.
--   By default, this will be 1 40% of the time and 0 the rest (ie. default aern behavior).
--   For multiple reraises, this can be set on spawn for more reraises.
--   To run a function when a reraise occurs, add a listener to AERN_RERAISE

require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.aern = xi.mix.aern or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.aern.braceletPlan = function(braceletActive, now)
    if braceletActive == 0 then
        return { active = 1, timer = now + 30, animationSub = 2, attp = 30, delayp = -33, acc = 40 }
    elseif braceletActive == 1 then
        return { active = 0, timer = now + 80, damageTaken = 0, animationSub = 1, attp = -30, delayp = 33, acc = -40 }
    end

    return nil
end

xi.mix.aern.damagePlan = function(braceletActive, damageTaken, amount)
    if braceletActive ~= 0 then
        return nil
    end

    damageTaken = damageTaken + amount
    return { damageTaken = damageTaken, shouldToggle = damageTaken >= 300 }
end

xi.mix.aern.shouldToggleTimer = function(now, braceletTimer)
    return now > braceletTimer
end

xi.mix.aern.petSpawnPlan = function(mainJob, hasPet, elementalCount)
    if hasPet then
        return nil
    end

    if mainJob == xi.job.SMN and elementalCount <= 5 then
        return { castElemental = true, elementalCount = elementalCount + 1 }
    elseif mainJob == xi.job.BST or mainJob == xi.job.DRG then
        return { callPet = true, inactiveTime = 3000, ignoreBusy = true }
    end

    return nil
end

xi.mix.aern.shouldRespawnPet = function(now, respawnTime, hasPet, mainJob)
    return respawnTime > 0 and now >= respawnTime and not hasPet and (mainJob == xi.job.BST or mainJob == xi.job.SMN)
end

xi.mix.aern.reraisePlan = function(reraises, currentReraise, roll, allowDrops)
    if reraises == 0 then reraises = 1 end
    if currentReraise >= reraises or roll <= 60 then return { reraise = false, noDrops = 0 } end
    return { reraise = true, noDrops = allowDrops == 0 and 1 or nil }
end

g_mixins.families.aern = function(aernMob)
    local petDeath = function(mob)
        local pet = mob:getPet()
        if pet then
            pet:addListener('DEATH', 'AERN_PET_DEATH', function(petMob, player, optParams)
                local master = petMob:getMaster()
                if master and master:isAlive() and master:isEngaged() then
                    master:setLocalVar('petRespawnTime', GetSystemTime() + 30)
                end
            end)
        end
    end

    -- Aerns only spawn on engage, we need to manually control the spawn behavior with a summoning animation
    local petSpawn = function(mob)
        local mainJob = mob:getMainJob()
        local plan = xi.mix.aern.petSpawnPlan(mainJob, mob:hasPet(), mob:getLocalVar('aernElementalCount'))

        if not plan then
            return
        end

        if plan.castElemental then
            -- SMN Aerns only spawn 5 elementals
            local elemental = math.random(xi.magic.spell.FIRE_SPIRIT, xi.magic.spell.DARK_SPIRIT)
            mob:castSpell(elemental, mob)
            mob:setLocalVar('aernElementalCount', plan.elementalCount)

            -- Add death listener to elemental when it spawns
            petDeath(mob)
        elseif plan.callPet then
            xi.mob.callPets(mob, nil, { inactiveTime = plan.inactiveTime, ignoreBusy = plan.ignoreBusy })

            -- Add death listener to pet when it spawns
            petDeath(mob)
        end
    end

    -- Bracelets activate after 80 seconds or 300 damage taken.
    -- They last for 30 seconds and give a 30% attack boost, +40 acc, and 33% delay reduction.
    local function toggleBracelets(mob)
        local braceletActive = mob:getLocalVar('braceletActive')
        local plan = xi.mix.aern.braceletPlan(braceletActive, GetSystemTime())

        if not plan then
            return
        end

        mob:setLocalVar('braceletTimer', plan.timer)
        mob:setLocalVar('braceletActive', plan.active)
        mob:setAnimationSub(plan.animationSub)

        if plan.damageTaken then
            mob:setLocalVar('braceletDamageTaken', plan.damageTaken)
        end

        if plan.active == 1 then
            mob:addMod(xi.mod.ATTP, plan.attp)
            mob:addMod(xi.mod.DELAYP, plan.delayp)
            mob:addMod(xi.mod.ACC, plan.acc)
        else
            mob:delMod(xi.mod.ACC, -plan.acc)
            mob:delMod(xi.mod.ATTP, -plan.attp)
            mob:delMod(xi.mod.DELAYP, -plan.delayp)
        end
    end

    -- Prevent BSTs and SMNs from summoning pets while idle
    aernMob:addListener('SPAWN', 'AERN_SPAWN', function(mob)
        if mob:getMainJob() == xi.job.BST then
            mob:setMobMod(xi.mobMod.SPECIAL_SKILL, 0)
        elseif mob:getMainJob() == xi.job.SMN then
            mob:setSpellList(0)
        end
    end)

    aernMob:addListener('ENGAGE', 'AERN_ENGAGE', function(mob)
        local petJobs = { xi.job.BST, xi.job.DRG, xi.job.SMN }
        mob:setLocalVar('braceletTimer', GetSystemTime() + 80)
        mob:setMagicCastingEnabled(true) -- Enable casting when engaged

        if utils.contains(mob:getMainJob(), petJobs) then
            mob:timer(3000, function(mobArg)
                petSpawn(mobArg)
            end)
        end
    end)

    -- Despawn pets when Aern is out of combat
    aernMob:addListener('ROAM_TICK', 'AERN_ROAM', function(mob)
        mob:setMagicCastingEnabled(false) -- Disable casting when idle
        mob:setAnimationSub(1)

        local pet = mob:getPet()
        if pet and pet:isSpawned() then
            DespawnMob(pet:getID())
        end

        if mob:getMainJob() == xi.job.BST then
            mob:setMobMod(xi.mobMod.SPECIAL_SKILL, 0)
        elseif mob:getMainJob() == xi.job.SMN then
            mob:setSpellList(0)
        end
    end)

    aernMob:addListener('COMBAT_TICK', 'AERN_COMBAT_TICK', function(mob)
        -- Pet respawn check
        local petRespawnTime = mob:getLocalVar('petRespawnTime')

        if xi.mix.aern.shouldRespawnPet(GetSystemTime(), petRespawnTime, mob:hasPet() and mob:getPet() ~= nil, mob:getMainJob()) then
            mob:setLocalVar('petRespawnTime', 0) -- Clear the timer
            petSpawn(mob)
        end

        -- Bracelet activation check
        local braceletTimer = mob:getLocalVar('braceletTimer')

        if xi.mix.aern.shouldToggleTimer(GetSystemTime(), braceletTimer) then
            toggleBracelets(mob)
        end
    end)

    aernMob:addListener('TAKE_DAMAGE', 'AERN_TAKE_DAMAGE', function(mob, amount, attacker, attackType, damageType)
        local braceletActive = mob:getLocalVar('braceletActive')

        -- Only track damage if not already using bracelets
        local plan = xi.mix.aern.damagePlan(braceletActive, mob:getLocalVar('braceletDamageTaken'), amount)
        if plan then
            mob:setLocalVar('braceletDamageTaken', plan.damageTaken)

            -- Check if 300 damage threshold reached
            if plan.shouldToggle then
                toggleBracelets(mob)
            end
        end
    end)

    aernMob:addListener('DEATH', 'AERN_DEATH', function(mob, killer)
        if not killer then
            return
        end

        local reraises    = mob:getLocalVar('AERN_RERAISE_MAX')
        local currReraise = mob:getLocalVar('AERN_RERAISES')

        local reraisePlan = xi.mix.aern.reraisePlan(reraises, currReraise, math.random(1, 100), mob:getLocalVar('ALLOW_DROPS'))
        if not reraisePlan.reraise then mob:setMobMod(xi.mobMod.NO_DROPS, reraisePlan.noDrops); return end
        if reraisePlan.noDrops then mob:setMobMod(xi.mobMod.NO_DROPS, reraisePlan.noDrops) end

        local target   = mob:getTarget()
        local targetID = 0
        local masterID = 0

        if target then
            targetID = target:getID()
        end

        if target:isPet() and target:getMaster() then
            masterID = target:getMaster():getID()
        end

        mob:timer(12000, function(mobArg)
            target = GetPlayerByID(targetID)

            if not target then
                -- try mob
                target = GetEntityByID(targetID, mobArg:getInstance(), true)
            end

            if
                not target and
                masterID > 0
            then
                target = GetPlayerByID(masterID)

                if not target then
                    -- try mob
                    target = GetEntityByID(masterID, mobArg:getInstance(), true)
                end
            end

            mobArg:setHP(mob:getMaxHP())
            mobArg:setAnimationSub(3)
            mobArg:setLocalVar('AERN_RERAISES', currReraise + 1)
            mobArg:resetAI()
            mobArg:stun(3000)

            if mob:getMainJob() == xi.job.SMN then
                mob:setLocalVar('aernElementalCount', 0)
            end

            if
                target and
                mobArg:checkDistance(target) < 25 and
                target:isAlive()
            then
                mobArg:updateClaim(target)
                mobArg:updateEnmity(target)
            else
                local partySize = killer:getPartySize() -- Check for other available valid aggro targets
                local i         = 1

                if killer then
                    for _, partyMember in pairs(killer:getAlliance()) do --TODO add enmity list check when binding avail
                        -- Engage
                        if partyMember:isAlive() and mobArg:checkDistance(partyMember) < 25 then
                            mobArg:updateClaim(partyMember)
                            mobArg:updateEnmity(partyMember)

                            break

                        -- Disengage.
                        elseif i == partySize then
                            mobArg:disengage()
                        end

                        i = i + 1
                    end
                else
                    mobArg:disengage()
                end
            end

            mobArg:triggerListener('AERN_RERAISE', mobArg, currReraise + 1)
        end)

        mob:timer(16000, function(mobArg)
            mobArg:setAnimationSub(1)
        end)
    end)
end

return g_mixins.families.aern
