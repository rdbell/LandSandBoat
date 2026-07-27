-----------------------------------
-- Pet Global Functions
-----------------------------------
require('scripts/globals/nyzul/pathos')
-----------------------------------
xi = xi or {}
xi.pet = xi.pet or {}

local avatarPetIDs = set
{
    xi.petId.CARBUNCLE,
    xi.petId.FENRIR,
    xi.petId.IFRIT,
    xi.petId.TITAN,
    xi.petId.LEVIATHAN,
    xi.petId.GARUDA,
    xi.petId.SHIVA,
    xi.petId.RAMUH,
    xi.petId.DIABOLOS,
    xi.petId.ALEXANDER,
    xi.petId.ODIN,
    xi.petId.ATOMOS,
    xi.petId.CAIT_SITH,
    xi.petId.SIREN,
}

local onMasterDeath = function(mob)
    local pet = mob:getPet()
    if pet ~= nil and pet:isAlive() then
        if xi.pet.shouldDespawnOnMasterDeath(true, true, pet:isEngaged()) then
            DespawnMob(pet:getID(), 2)
        end
    end
end

local astralOnlySpellIDs = set
{
    xi.magic.spell.ODIN,
    xi.magic.spell.ALEXANDER,
}

-- Pure Beastmaster cleanup gate after the master-death listener reads pet state.
xi.pet.shouldDespawnOnMasterDeath = function(hasPet, petAlive, petEngaged)
    return hasPet and petAlive and not petEngaged
end

-- Pure non-PC spawnPet state effects after the host reads state identity.
xi.pet.spawnPetStatePlan = function(isPC, hasState, isCallBeast)
    if isPC or not hasState then
        return {}
    end

    return
    {
        suppressStateMessage = true,
        addMasterDeathListener = isCallBeast,
        addMasterDespawnListener = isCallBeast,
    }
end

-- Pure avatar-specific post-spawn requests after host state reads.
xi.pet.avatarSpawnPlan = function(isAvatar, hasFavor, petID, hasTarget, hasSpawnedPet)
    local plan = { timers = {} }
    if not isAvatar then
        return plan
    end

    if hasFavor then
        plan.resetFavor = true
        plan.applyFavorAura = true
        plan.applyFavorDebuffs = true
    end

    if petID == xi.petId.ALEXANDER and hasSpawnedPet then
        table.insert(plan.timers, { delay = 5000, ability = xi.jobAbility.PERFECT_DEFENSE, target = 'pet' })
    elseif petID == xi.petId.ODIN and hasTarget then
        plan.attackTarget = true
    elseif petID == xi.petId.ATOMOS and hasTarget and hasSpawnedPet then
        table.insert(plan.timers, { delay = 3000, ability = xi.jobAbility.DECONSTRUCTION, target = 'target' })
        table.insert(plan.timers, { delay = 10000, ability = xi.jobAbility.CHRONOSHIFT, target = 'pet' })
    end

    return plan
end

-- Summoning mob skills require an assigned pet that is not already spawned.
xi.pet.mobSkillCheckResult = function(hasAssignedPet, hasSpawnedPet)
    if not hasAssignedPet or hasSpawnedPet then
        return 1
    end

    return 0
end

-- Ordered casting admission after the branch-specific host reads.
xi.pet.castingCheckResult = function(hasSpawnedPet, astralOnly, hasAstralFlow, canUsePet, isPC, avatarMiniFightResult, hasAssignedPet)
    if hasSpawnedPet then
        return xi.msg.basic.ALREADY_HAS_A_PET
    elseif astralOnly and not hasAstralFlow then
        return xi.msg.basic.MAGIC_MUST_ASTRAL_FLOW
    elseif not canUsePet then
        return xi.msg.basic.CANT_BE_USED_IN_AREA
    elseif isPC then
        return avatarMiniFightResult
    end

    return hasAssignedPet and 0 or 1
end

-- Pure Familiar-buff eligibility after the host reads pet state.
xi.pet.familiarBuffsShouldApply = function(ownerPresent, petPresent, petAlive, hasFamiliarBuffs)
    return ownerPresent and petPresent and petAlive and not hasFamiliarBuffs
end

-- Pure Familiar bonuses after the host checks eligibility.
xi.pet.familiarBuffPlan = function(familiarBonus, extendCharm, petMaxHP)
    local plan =
    {
        extendCharm = extendCharm,
        hasteAbility = familiarBonus > 0 and familiarBonus * 100 or 0,
        addedHP = petMaxHP * 0.1,
    }

    if extendCharm then
        local bonusSeconds = familiarBonus * 60
        plan.charmMinSeconds = 25 * 60 + bonusSeconds
        plan.charmMaxSeconds = 30 * 60 + bonusSeconds
    end

    return plan
end

-- Pure setMobPet admission and entity-action plan after host state reads.
xi.pet.setMobPetPlan = function(mobPresent, mobIsMob, petFound, nameMatches, petHasMaster, mobHasPet, mobSpawned, petSpawned)
    if
        not mobPresent or
        not mobIsMob or
        not petFound or
        not nameMatches or
        petHasMaster or
        mobHasPet
    then
        return {}
    end

    return
    {
        disallowRespawn = true,
        despawnPet = not mobSpawned and petSpawned,
        linkPet = true,
    }
end

---@param target CBaseEntity
---@param mob CBaseEntity
---@param skill CMobSkill
---@return number
xi.pet.onMobSkillCheck = function(target, mob, skill)
    -- block mobskill if mob doesn't have an assigned pet or pet is currently spawned
    return xi.pet.mobSkillCheckResult(mob:getPet() ~= nil, mob:hasPet())
end

---@param caster CBaseEntity
---@param target CBaseEntity
---@param spell CSpell
---@return number
xi.pet.onCastingCheck = function(caster, target, spell)
    if caster:hasPet() then
        return xi.pet.castingCheckResult(true, false, false, true, false, 0, false)
    elseif
        astralOnlySpellIDs[spell:getID()] and
        not caster:hasStatusEffect(xi.effect.ASTRAL_FLOW)
    then
        return xi.pet.castingCheckResult(false, true, false, true, false, 0, false)
    elseif not caster:canUseMisc(xi.zoneMisc.PET) then
        return xi.pet.castingCheckResult(false, false, false, false, false, 0, false)
    elseif caster:getObjType() == xi.objType.PC then
        return xi.pet.castingCheckResult(false, false, false, true, true, xi.summon.avatarMiniFightCheck(caster, spell:getID()), false)
    end

    -- non-PC without an attached pet.
    return xi.pet.castingCheckResult(false, false, false, true, false, 0, caster:getPet() ~= nil)
end

---@param caster CBaseEntity
---@param petID number?
---@param state CSpell|CMobSkill?
---@param target CBaseEntity?
---@return nil
xi.pet.spawnPet = function(caster, petID, state, target)
    caster:spawnPet(petID)

    -- mobs don't emit message when using call beast/wyvern, activate, or summoner spells
    if caster:getObjType() ~= xi.objType.PC and state then
        local plan = xi.pet.spawnPetStatePlan(false, true, false)
        if plan.suppressStateMessage then
            state:setMsg(xi.msg.basic.NONE)
        end

        if state:getID() == xi.mobSkill.CALL_BEAST then
            plan = xi.pet.spawnPetStatePlan(false, true, true)
            -- bst mob pets despawn if not engaged when owner leaves
            if plan.addMasterDeathListener then
                caster:addListener('DEATH', 'BEASTMASTER_DEATH', onMasterDeath)
            end
            if plan.addMasterDespawnListener then
                caster:addListener('DESPAWN', 'BEASTMASTER_DESPAWN', onMasterDeath)
            end
        end
    end

    if avatarPetIDs[petID] then
        local effect = caster:getStatusEffect(xi.effect.AVATARS_FAVOR)
        local pet = nil
        if petID == xi.petId.ALEXANDER or (petID == xi.petId.ATOMOS and target) then
            pet = caster:getPet()
        end

        local plan = xi.pet.avatarSpawnPlan(true, effect ~= nil, petID, target ~= nil, pet ~= nil)
        if plan.resetFavor then
            effect:setPower(1) -- resummon resets effect
        end
        if plan.applyFavorAura then
            xi.avatarsFavor.applyAvatarsFavorAuraToPet(caster, effect)
        end
        if plan.applyFavorDebuffs then
            xi.avatarsFavor.applyAvatarsFavorDebuffsToPet(caster)
        end

        if plan.attackTarget then
            caster:petAttack(target)
        end

        if pet then
            for _, timer in ipairs(plan.timers) do
                local timerRequest = timer
                pet:timer(timerRequest.delay, function()
                    local abilityTarget = timerRequest.target == 'pet' and pet or target
                    pet:usePetAbility(timerRequest.ability, abilityTarget)
                end)
            end
        end
    end

    -- Nyzul Isle has Pathos set randomly on floors and is recorded as bits in a localvar of the instance
    if caster:getZoneID() == xi.zone.NYZUL_ISLE then
        xi.nyzul.addPetSpawnPathos(caster)
    end
end

-- TODO should charmed entities lose their buffs when they become uncharmed?
xi.pet.applyFamiliarBuffs = function(owner, pet)
    -- Keep these host reads short-circuited; FamiliarBuffsShouldApply provides
    -- the pure equivalent after they have been read.
    if not owner or not pet then
        return
    end

    local petAlive = pet:isAlive()
    if not petAlive then
        return
    end

    local hasFamiliarBuffs = pet:getLocalVar('hasFamiliarBuffs') ~= 0
    if not xi.pet.familiarBuffsShouldApply(true, true, petAlive, hasFamiliarBuffs) then
        return
    end

    pet:setLocalVar('hasFamiliarBuffs', 1)

    local familiarBonus = owner:getMod(xi.mod.FAMILIAR_BONUS)
    local plan = xi.pet.familiarBuffPlan(familiarBonus, owner:isPC() and pet:isCharmed(), pet:getMaxHP())
    if plan.extendCharm then
        pet:extendCharm(plan.charmMinSeconds, plan.charmMaxSeconds)
    end

    if plan.hasteAbility > 0 then
        pet:addMod(xi.mod.HASTE_ABILITY, plan.hasteAbility)
    end

    pet:setMaxHP(pet:getMaxHP() + plan.addedHP) -- technically BASE_HP mod is added back to generate modhp, but close enough
    -- wakes up pets
    pet:addHP(plan.addedHP)

    -- TODO does familiar give some bonus resistance to crowd control? Is it only for mob pets?
    -- Lots of reports of mobs using Familiar and the pet having higher chance to resist bind/sleep/etc
end

-- Assigns a pet to the "mob" parameter by adding "offset" to the mob's ID
-- will bail out if the offset mob's name doesn't match "petName" parameter, as a sanity check for ID shifts or mobs that have multiple job types in the same zone
---@param mob CBaseEntity
---@param offset number
---@param petName string
---@return nil
xi.pet.setMobPet = function(mob, offset, petName)
    if not mob or mob:getObjType() ~= xi.objType.MOB then
        return
    end

    local pet = GetMobByID(mob:getID() + offset)
    if not pet or pet:getName() ~= petName then
        return
    end

    if pet:getMaster() then
        return
    end

    if mob:getPet() then
        return
    end

    local plan = xi.pet.setMobPetPlan(true, true, true, true, false, false, mob:isSpawned(), pet:isSpawned())
    if plan.disallowRespawn then
        -- pet is always spawned by master
        DisallowRespawn(pet:getID(), true)
    end

    if plan.despawnPet then
        DespawnMob(pet:getID(), 2)
    end

    if plan.linkPet then
        -- link mob and pet for things like call_beast, summon elemental, etc
        mob:setPet(pet)
    end
end
