require('scripts/globals/mixins')

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.rampart = xi.mix.rampart or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.rampart.engagePlan = function(animationSub)
    return { stun = animationSub == 0, animationSub = 1, swapTime = 30 }
end

xi.mix.rampart.roamPlan = function(engaged, timedSpawn, animationSub)
    if engaged then
        return nil
    elseif timedSpawn == 0 and animationSub == 1 then
        return { stun = true, animationSub = 0 }
    elseif timedSpawn > 0 and animationSub == 0 then
        return { stun = true, animationSub = 1 }
    end
end

xi.mix.rampart.combatPlan = function(time, swapTime, prevented, animationSub)
    if time <= swapTime or prevented then
        return nil
    elseif animationSub == 0 then
        return { stun = true, animationSub = 1, swapTime = time + 30 }
    elseif animationSub == 1 then
        return { stun = true, animationSub = 0, swapTime = time + 9 }
    end
end

xi.mix.rampart.shouldDisableTimedSpawn = function(spawnAvailable, numberSpawned, spawnLimit)
    return not spawnAvailable or (numberSpawned >= spawnLimit and spawnLimit ~= 0)
end

xi.mix.rampart.nextPetTime = function(now, roll)
    return (roll * 5) + 45 + now
end

xi.mix.rampart.nextPetAfterSpawn = function(timedSpawn, nextPet, roll)
    if timedSpawn > 1 then return timedSpawn + nextPet end
    return xi.mix.rampart.nextPetTime(nextPet, roll)
end

xi.mix.rampart.isPetDue = function(now, nextPet) return now > nextPet end

g_mixins.families.rampart = function(rampartMob)
    -- AnimationSub for Ramparts
    local doorClosed = 0
    local doorOpen   = 1

    rampartMob:addListener('ENGAGE', 'RAMPART_ENGAGED', function(mob, target)
        local plan = xi.mix.rampart.engagePlan(mob:getAnimationSub())
        if plan.stun then
            mob:stun(3000)
        end
        mob:setAnimationSub(plan.animationSub)
        mob:setLocalVar('swapTime', plan.swapTime)
    end)

    rampartMob:addListener('ROAM_TICK', 'RAMPART_TICK', function(mob)
        -- Function to allow Reinforcements to work in a timed scenario (Idle).
        -- Set local var in onMobSpawn to what the reuse is to activate function.
        -- example: (mob:setLocalVar('timedSpawn', 60)) for 60 second intervals
        local timedSpawn = mob:getLocalVar('timedSpawn')
        local roamPlan = xi.mix.rampart.roamPlan(mob:isEngaged(), timedSpawn, mob:getAnimationSub())
        -- Can only have doors open if in combat or if it is a timed Reinforcements type,
        -- as it can only use the mobSkill with the doors open.
        if roamPlan then
            mob:stun(3000)
            mob:setAnimationSub(roamPlan.animationSub)
        end

        if mob:isEngaged() then return end

        if timedSpawn > 0 and not mob:isEngaged() then
            local instance   = mob:getInstance()
            local count      = mob:getLocalVar('spawnCount')
            local mobID      = mob:getID()
            local time       = GetSystemTime()
            local limit      = mob:getLocalVar('spawnLimit')

            -- if all mobs are spawned then set the timer to 0 and return
            local spawnAvail = false
            for number = 1, count do
                if not GetMobByID(mobID + number, instance):isAlive() then
                    spawnAvail = true
                    break
                end
            end

            if xi.mix.rampart.shouldDisableTimedSpawn(spawnAvail, mob:getLocalVar('numberSpawned'), limit) then
                mob:setLocalVar('timedSpawn', 0)
                return
            end

            if not mob:hasPreventActionEffect() then
                if mob:getLocalVar('nextPet') == 0 then
                    mob:setLocalVar('nextPet', xi.mix.rampart.nextPetTime(time, math.random(0, 3)))
                end

                local nextPet = mob:getLocalVar('nextPet')

                if xi.mix.rampart.isPetDue(time, nextPet) then
                    for number = 1, count do
                        local add = GetMobByID(mobID + number, instance)
                        if add and not add:isSpawned() then
                            add:setLocalVar('masterID', mobID)
                            mob:setLocalVar('nextPet', xi.mix.rampart.nextPetAfterSpawn(timedSpawn, nextPet, math.random(0, 3)))

                            mob:useMobAbility(2034)
                            -- Setup add with var with masters ID to allow respawning on death of it.
                            add:addListener('SPAWN', 'RAMPART_ADD', function(addArg)
                                addArg:setLocalVar('masterID', mobID)
                            end)

                            add:addListener('DEATH', 'RAMPART_ADD_DEATH', function(petArg, killer)
                                local masterID = petArg:getLocalVar('masterID')

                                if masterID > 0 then
                                    local rampartArg = GetMobByID(masterID, petArg:getInstance())
                                    if rampartArg then
                                        local limitArg   = rampartArg:getLocalVar('spawnLimit')
                                        if
                                            rampartArg:isAlive() and
                                            rampartArg:getLocalVar('timedSpawn') == 0 and
                                            (rampartArg:getLocalVar('numberSpawned') < limitArg or limitArg == 0)
                                        then
                                            rampartArg:setLocalVar('nextPet', 0)
                                            rampartArg:setLocalVar('timedSpawn', (math.random(0, 3) * 5) + 45)
                                        end

                                        petArg:removeListener('RAMPART_ADD')
                                    end
                                end
                            end)

                            break
                        end
                    end
                end
            end
        end
    end)

    rampartMob:addListener('COMBAT_TICK', 'RAMPART_COMBAT', function(mob)
        local time     = mob:getBattleTime()
        local swapTime = mob:getLocalVar('swapTime')

        local plan = xi.mix.rampart.combatPlan(time, swapTime, mob:hasPreventActionEffect(), mob:getAnimationSub())
        if plan then
            mob:stun(3000)
            mob:setAnimationSub(plan.animationSub)
            mob:setLocalVar('swapTime', plan.swapTime)
        end
    end)
end

return g_mixins.families.rampart
