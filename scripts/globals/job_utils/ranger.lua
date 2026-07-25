-----------------------------------
-- Ranger Job Utilities
-- Dual-wired pure inject forms (slice 6746 / 0902):
--   one-hour recast, sharpshot/fixed effect params, scavenge arrows,
--   camouflage duration, shadowbind duration/success/msg,
--   EES/shadowbind/bounty check gates, animation offsets
-- Parity: internal/ranger
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.ranger = xi.job_utils.ranger or {}


-----------------------------------
-- Pure inject pins (internal/ranger, slice 6746 / 0902)
-----------------------------------
xi.job_utils.ranger.oneHourRecastSecondsPerMod   = 60
xi.job_utils.ranger.sharpshotBasePower           = 40
xi.job_utils.ranger.sharpshotDuration            = 60
xi.job_utils.ranger.velocityShotPower            = 1
xi.job_utils.ranger.velocityShotDuration         = 7200
xi.job_utils.ranger.barrageDuration              = 60
xi.job_utils.ranger.unlimitedShotPower           = 1
xi.job_utils.ranger.unlimitedShotDuration        = 60
xi.job_utils.ranger.flashyShotPower              = 1
xi.job_utils.ranger.flashyShotDuration           = 60
xi.job_utils.ranger.stealthShotPower             = 1
xi.job_utils.ranger.stealthShotDuration          = 60
xi.job_utils.ranger.doubleShotPower              = 40
xi.job_utils.ranger.doubleShotDuration           = 90
xi.job_utils.ranger.decoyShotPower               = 11
xi.job_utils.ranger.decoyShotDuration            = 30
xi.job_utils.ranger.decoyShotTick                = 1
xi.job_utils.ranger.overkillPower                = 11
xi.job_utils.ranger.overkillDuration             = 60
xi.job_utils.ranger.overkillTick                 = 1
xi.job_utils.ranger.camouflagePower              = 1
xi.job_utils.ranger.camouflageBaseRollMin        = 30
xi.job_utils.ranger.camouflageBaseRollMax        = 300
xi.job_utils.ranger.shadowbindBaseDuration       = 30
xi.job_utils.ranger.scavengeArrowsUsedMod        = 10000
xi.job_utils.ranger.scavengeArrowsMax            = 99
xi.job_utils.ranger.scavengeLevelScale           = 200
xi.job_utils.ranger.scavengeBonusScale           = 100
xi.job_utils.ranger.skillArchery                 = 25
xi.job_utils.ranger.skillMarksmanship            = 26
xi.job_utils.ranger.skillThrowing                = 27
xi.job_utils.ranger.msgScavengeFindNothing       = 139
xi.job_utils.ranger.msgScavengeFindItem          = 140
xi.job_utils.ranger.msgJAMiss                    = 158
xi.job_utils.ranger.msgNoRangedWeapon            = 216
xi.job_utils.ranger.msgIsEffect                  = 277
xi.job_utils.ranger.msgCannotAttackTarget        = 446
xi.job_utils.ranger.msgScavengeFindItems         = 674

-- Pure: OneHourRecast
xi.job_utils.ranger.oneHourRecastFromParams = function(params)
    params = params or {}
    local out = (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.ranger.oneHourRecastSecondsPerMod
    if out < 0 then
        return 0
    end

    return out
end

xi.job_utils.ranger.sharpshotPowerFromParams = function(sharpshotMod)
    return xi.job_utils.ranger.sharpshotBasePower + (sharpshotMod or 0)
end

xi.job_utils.ranger.sharpshotFromParams = function(params)
    params = params or {}
    return {
        power    = xi.job_utils.ranger.sharpshotPowerFromParams(params.sharpshotMod),
        duration = xi.job_utils.ranger.sharpshotDuration,
    }
end

xi.job_utils.ranger.velocityShotFromParams = function()
    return {
        power    = xi.job_utils.ranger.velocityShotPower,
        duration = xi.job_utils.ranger.velocityShotDuration,
    }
end

xi.job_utils.ranger.barrageFromParams = function()
    return { power = 0, duration = xi.job_utils.ranger.barrageDuration }
end

xi.job_utils.ranger.unlimitedShotFromParams = function()
    return {
        power    = xi.job_utils.ranger.unlimitedShotPower,
        duration = xi.job_utils.ranger.unlimitedShotDuration,
    }
end

xi.job_utils.ranger.flashyShotFromParams = function()
    return {
        power    = xi.job_utils.ranger.flashyShotPower,
        duration = xi.job_utils.ranger.flashyShotDuration,
    }
end

xi.job_utils.ranger.stealthShotFromParams = function()
    return {
        power    = xi.job_utils.ranger.stealthShotPower,
        duration = xi.job_utils.ranger.stealthShotDuration,
    }
end

xi.job_utils.ranger.doubleShotFromParams = function()
    return {
        power    = xi.job_utils.ranger.doubleShotPower,
        duration = xi.job_utils.ranger.doubleShotDuration,
    }
end

xi.job_utils.ranger.decoyShotFromParams = function()
    return {
        power    = xi.job_utils.ranger.decoyShotPower,
        duration = xi.job_utils.ranger.decoyShotDuration,
        tick     = xi.job_utils.ranger.decoyShotTick,
    }
end

xi.job_utils.ranger.overkillFromParams = function()
    return {
        power    = xi.job_utils.ranger.overkillPower,
        duration = xi.job_utils.ranger.overkillDuration,
        tick     = xi.job_utils.ranger.overkillTick,
    }
end

-- Pure: DecodeArrowsUsed — returns arrowID, arrowsUsedLow
xi.job_utils.ranger.decodeArrowsUsedFromParams = function(arrowsUsed)
    arrowsUsed = arrowsUsed or 0
    local mod = xi.job_utils.ranger.scavengeArrowsUsedMod
    return math.floor(arrowsUsed / mod), arrowsUsed % mod
end

-- Pure: ScavengeArrowsToReturn
xi.job_utils.ranger.scavengeArrowsToReturnFromParams = function(params)
    params = params or {}
    local bonuses = ((params.scavengeMod or 0) + (params.scavengeMerit or 0))
        / xi.job_utils.ranger.scavengeBonusScale
    local factor = (params.mainLvl or 0) / xi.job_utils.ranger.scavengeLevelScale + bonuses
    local n = math.floor((params.arrowsUsedLow or 0) * factor)
    if n < 0 then
        return 0
    end

    if n > xi.job_utils.ranger.scavengeArrowsMax then
        return xi.job_utils.ranger.scavengeArrowsMax
    end

    return n
end

xi.job_utils.ranger.scavengeMessageFromParams = function(arrowsToReturn)
    arrowsToReturn = arrowsToReturn or 0
    if arrowsToReturn <= 0 then
        return xi.job_utils.ranger.msgScavengeFindNothing
    elseif arrowsToReturn == 1 then
        return xi.job_utils.ranger.msgScavengeFindItem
    end

    return xi.job_utils.ranger.msgScavengeFindItems
end

-- Pure: CamouflageDuration
xi.job_utils.ranger.camouflageDurationFromParams = function(params)
    params = params or {}
    local duration = (params.baseRoll or 0)
        * (1 + 0.01 * (params.camouflageDurationMod or 0))
    return math.floor(duration * (params.sneakInvisMultiplier or 1))
end

-- Pure: ShadowbindDuration / Success / Msg
xi.job_utils.ranger.shadowbindDurationFromParams = function(params)
    params = params or {}
    return xi.job_utils.ranger.shadowbindBaseDuration
        + (params.shadowBindExt or 0)
        + (params.jpDuration or 0)
end

xi.job_utils.ranger.shadowbindSuccessFromParams = function(params)
    params = params or {}
    if params.alreadyBound then
        return false
    end

    return (params.roll0to99 or 0) >= (params.bindMeva or 0)
end

xi.job_utils.ranger.shadowbindMsgFromParams = function(success)
    if success then
        return xi.job_utils.ranger.msgIsEffect
    end

    return xi.job_utils.ranger.msgJAMiss
end

-- Pure: skill helpers
xi.job_utils.ranger.isRangedCombatSkillFromParams = function(skill)
    skill = skill or 0
    local archery = xi.skill and xi.skill.ARCHERY or xi.job_utils.ranger.skillArchery
    local marks = xi.skill and xi.skill.MARKSMANSHIP or xi.job_utils.ranger.skillMarksmanship
    local throw = xi.skill and xi.skill.THROWING or xi.job_utils.ranger.skillThrowing
    return skill == archery or skill == marks or skill == throw
end

xi.job_utils.ranger.hasMatchingRangedPairFromParams = function(params)
    params = params or {}
    local archery = xi.skill and xi.skill.ARCHERY or xi.job_utils.ranger.skillArchery
    local marks = xi.skill and xi.skill.MARKSMANSHIP or xi.job_utils.ranger.skillMarksmanship
    local r = params.rangedSkill or 0
    local a = params.ammoSkill or 0
    return (r == marks and a == marks) or (r == archery and a == archery)
end

-- Pure: CheckEagleEyeShot — returns msg, ok
xi.job_utils.ranger.checkEagleEyeShotFromParams = function(params)
    params = params or {}
    if params.hasRangedWeapon and xi.job_utils.ranger.isRangedCombatSkillFromParams(params.skillType) then
        if params.hasAmmoWeapon or params.skillType == (xi.skill and xi.skill.THROWING or xi.job_utils.ranger.skillThrowing) then
            return 0, true
        end
    end

    return xi.job_utils.ranger.msgNoRangedWeapon, false
end

xi.job_utils.ranger.checkShadowbindFromParams = function(params)
    params = params or {}
    if xi.job_utils.ranger.hasMatchingRangedPairFromParams(params) then
        return 0, true
    end

    return xi.job_utils.ranger.msgNoRangedWeapon, false
end

xi.job_utils.ranger.checkBountyShotFromParams = function(params)
    params = params or {}
    if not params.isMob then
        return xi.job_utils.ranger.msgCannotAttackTarget, false
    end

    if xi.job_utils.ranger.hasMatchingRangedPairFromParams(params) then
        return 0, true
    end

    return xi.job_utils.ranger.msgNoRangedWeapon, false
end

xi.job_utils.ranger.marksmanAnimOffsetFromParams = function(rangedSkill)
    local marks = xi.skill and xi.skill.MARKSMANSHIP or xi.job_utils.ranger.skillMarksmanship
    if rangedSkill == marks then
        return 1
    end

    return 0
end

xi.job_utils.ranger.bountyShotAnimOffsetFromParams = function(rangedSkill)
    local archery = xi.skill and xi.skill.ARCHERY or xi.job_utils.ranger.skillArchery
    if rangedSkill == archery then
        return -1
    end

    return 0
end

-----------------------------------
-- Helper Functions
-----------------------------------

-- TODO: Remove this logic when Fire and Brimstone quest is converted to IF
xi.job_utils.ranger.tryScavengeQuestItem = function(player)
    local fireAndBrimstoneCS = player:getCharVar('fireAndBrimstone')

    if
        player:getZoneID() == xi.zone.CASTLE_OZTROJA and
        fireAndBrimstoneCS == 5 and
        not player:hasItem(xi.item.OLD_EARRING) and
        player:getYPos() > -43 and player:getYPos() < -38 and
        player:getXPos() > -85 and player:getXPos() < -73 and
        player:getZPos() > -85 and player:getZPos() < -75 and
        math.random(1, 100) <= 50
    then
        npcUtil.giveItem(player, xi.item.OLD_EARRING)

        return true
    end

    return false
end

-----------------------------------
-- Ability Check Functions
-----------------------------------

xi.job_utils.ranger.checkEagleEyeShot = function(player, target, ability)
    local ranged = player:getStorageItem(0, 0, xi.slot.RANGED)
    local ammo   = player:getStorageItem(0, 0, xi.slot.AMMO)
    local skilltype = 0
    local hasRangedWeapon = ranged and ranged:isType(xi.itemType.WEAPON)
    local hasAmmoWeapon = ammo and ammo:isType(xi.itemType.WEAPON)

    if hasRangedWeapon then
        skilltype = ranged:getSkillType()
    end

    local msg, ok = xi.job_utils.ranger.checkEagleEyeShotFromParams({
        hasRangedWeapon = hasRangedWeapon,
        skillType       = skilltype,
        hasAmmoWeapon   = hasAmmoWeapon,
    })
    if ok then
        ability:setRecast(xi.job_utils.ranger.oneHourRecastFromParams({
            abilityRecast    = ability:getRecast(),
            oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
        }))
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.ranger.checkVelocityShot = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkSharpshot = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkScavenge = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkCamouflage = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkBarrage = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkShadowbind = function(player, target, ability)
    local msg, ok = xi.job_utils.ranger.checkShadowbindFromParams({
        rangedSkill = player:getWeaponSkillType(xi.slot.RANGED),
        ammoSkill   = player:getWeaponSkillType(xi.slot.AMMO),
    })
    if ok then
        return 0, 0
    end

    return msg, 0 -- You do not have an appropriate ranged weapon equipped.
end

xi.job_utils.ranger.checkUnlimitedShot = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkFlashyShot = function(player, target, ability)
    return 0, 0 -- Not implemented yet
end

xi.job_utils.ranger.checkStealthShot = function(player, target, ability)
    return 0, 0 -- Not implemented yet
end

xi.job_utils.ranger.checkDoubleShot = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkBountyShot = function(player, target, ability)
    local msg, ok = xi.job_utils.ranger.checkBountyShotFromParams({
        isMob       = target:getObjType() == xi.objType.MOB,
        rangedSkill = player:getWeaponSkillType(xi.slot.RANGED),
        ammoSkill   = player:getWeaponSkillType(xi.slot.AMMO),
    })
    if ok then
        return 0, 0
    end

    return msg, 0
end

xi.job_utils.ranger.checkDecoyShot = function(player, target, ability)
    return 0, 0
end

xi.job_utils.ranger.checkHoverShot = function(player, target, ability)
    return 0, 0  -- Not implemented yet
end

xi.job_utils.ranger.checkOverkill = function(player, target, ability)
    ability:setRecast(xi.job_utils.ranger.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))

    return 0, 0
end

-----------------------------------
-- Ability Use Functions
-----------------------------------

xi.job_utils.ranger.useEagleEyeShot = function(player, target, ability, action)
    local animOff = xi.job_utils.ranger.marksmanAnimOffsetFromParams(
        player:getWeaponSkillType(xi.slot.RANGED)
    )
    if animOff ~= 0 then
        action:setAnimation(target:getID(), action:getAnimation(target:getID()) + animOff)
    end

    local params = {}

    params.numHits = 1
    params.ignoreShadows = true -- Eagle Eye Shot bypasses Utsusemi and Blink

    -- TP params.
    local tp          = 1000 -- to ensure ftp multiplier is applied
    params.ftpMod     = { 5.0, 5.0, 5.0 }
    params.critVaries = { 0.0, 0.0, 0.0 }

    -- Stat params.
    params.str_wsc = 0
    params.dex_wsc = 0
    params.vit_wsc = 0
    params.agi_wsc = 0
    params.int_wsc = 0
    params.mnd_wsc = 0
    params.chr_wsc = 0

    params.enmityMult = 0.5

    -- Job Point Bonus Damage
    local jpValue = player:getJobPointLevel(xi.jp.EAGLE_EYE_SHOT_EFFECT)
    player:addMod(xi.mod.ALL_WSDMG_ALL_HITS, jpValue * 3)

    local damage, _, tpHits, extraHits = xi.weaponskills.doRangedWeaponskill(player, target, 0, params, tp, action, true)

    -- Set the message id ourselves
    if tpHits + extraHits > 0 then
        action:messageID(target:getID(), xi.msg.basic.JA_DAMAGE)
    else
        action:messageID(target:getID(), xi.msg.basic.JA_MISS_2)
    end

    return damage
end

xi.job_utils.ranger.useVelocityShot = function(player, target, ability, action)
    local p = xi.job_utils.ranger.velocityShotFromParams()
    player:addStatusEffect(xi.effect.VELOCITY_SHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.VELOCITY_SHOT
end

xi.job_utils.ranger.useSharpshot = function(player, target, ability, action)
    local p = xi.job_utils.ranger.sharpshotFromParams({
        sharpshotMod = player:getMod(xi.mod.SHARPSHOT),
    })
    player:addStatusEffect(xi.effect.SHARPSHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.SHARPSHOT
end

xi.job_utils.ranger.useScavenge = function(player, target, ability, action)
    -- RNG AF2 quest check
    if xi.job_utils.ranger.tryScavengeQuestItem(player) then
        return
    end

    local arrowsUsed = player:getLocalVar('ArrowsUsed')
    local arrowID, arrowsUsedLow = xi.job_utils.ranger.decodeArrowsUsedFromParams(arrowsUsed)
    local arrowsToReturn = xi.job_utils.ranger.scavengeArrowsToReturnFromParams({
        arrowsUsedLow = arrowsUsedLow,
        mainLvl       = player:getMainLvl(),
        scavengeMod   = player:getMod(xi.mod.SCAVENGE_EFFECT),
        scavengeMerit = player:getMerit(xi.merit.SCAVENGE_EFFECT),
    })
    local playerID = target:getID()
    local msg = xi.job_utils.ranger.scavengeMessageFromParams(arrowsToReturn)

    if arrowsToReturn == 0 then
        action:messageID(playerID, msg)
    else
        player:addItem(arrowID, arrowsToReturn)

        if arrowsToReturn == 1 then
            action:messageID(playerID, msg)
        else
            action:messageID(playerID, msg)
            action:additionalEffect(playerID, 1)
            action:addEffectParam(playerID, arrowsToReturn)
        end

        player:setLocalVar('ArrowsUsed', 0)
        return arrowID
    end
end

xi.job_utils.ranger.useCamouflage = function(player, target, ability, action)
    local baseRoll = math.random(
        xi.job_utils.ranger.camouflageBaseRollMin,
        xi.job_utils.ranger.camouflageBaseRollMax
    )
    local duration = xi.job_utils.ranger.camouflageDurationFromParams({
        baseRoll               = baseRoll,
        camouflageDurationMod  = player:getMod(xi.mod.CAMOUFLAGE_DURATION),
        sneakInvisMultiplier   = xi.settings.main.SNEAK_INVIS_DURATION_MULTIPLIER,
    })
    player:addStatusEffect(xi.effect.CAMOUFLAGE, {
        power    = xi.job_utils.ranger.camouflagePower,
        duration = duration,
        origin   = player,
    })

    return xi.effect.CAMOUFLAGE
end

xi.job_utils.ranger.useBarrage = function(player, target, ability, action)
    local p = xi.job_utils.ranger.barrageFromParams()
    player:addStatusEffect(xi.effect.BARRAGE, {
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.BARRAGE
end

xi.job_utils.ranger.useShadowbind = function(player, target, ability, action)
    local animOff = xi.job_utils.ranger.marksmanAnimOffsetFromParams(
        player:getWeaponSkillType(xi.slot.RANGED)
    )
    if animOff ~= 0 then -- can't have your crossbow/gun held like a bow, now can we?
        action:setAnimation(target:getID(), action:getAnimation(target:getID()) + animOff)
    end

    local duration = xi.job_utils.ranger.shadowbindDurationFromParams({
        shadowBindExt = player:getMod(xi.mod.SHADOW_BIND_EXT),
        jpDuration    = player:getJobPointLevel(xi.jp.SHADOWBIND_DURATION),
    })

    -- TODO: Acc penalty for /RNG, acc vs. mob level?
    local success = xi.job_utils.ranger.shadowbindSuccessFromParams({
        bindMeva     = target:getMod(xi.mod.BIND_MEVA),
        roll0to99    = math.random(0, 99),
        alreadyBound = target:hasStatusEffect(xi.effect.BIND),
    })
    ability:setMsg(xi.job_utils.ranger.shadowbindMsgFromParams(success))
    if success then
        target:addStatusEffect(xi.effect.BIND, { duration = duration, origin = player })
    end

    if xi.combat.ranged.shouldUseAmmo(player) then
        player:removeAmmo(1) -- Shadowbind depletes one round of ammo.
    end

    return xi.effect.BIND
end

xi.job_utils.ranger.useUnlimitedShot = function(player, target, ability, action)
    local p = xi.job_utils.ranger.unlimitedShotFromParams()
    player:addStatusEffect(xi.effect.UNLIMITED_SHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.UNLIMITED_SHOT
end

xi.job_utils.ranger.useFlashyShot = function(player, target, ability, action)
    -- TODO: Flashy Shot should add "D" damage to the next ranged attack
    local p = xi.job_utils.ranger.flashyShotFromParams()
    player:addStatusEffect(xi.effect.FLASHY_SHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.FLASHY_SHOT
end

xi.job_utils.ranger.useStealthShot = function(player, target, ability, action)
    local p = xi.job_utils.ranger.stealthShotFromParams()
    player:addStatusEffect(xi.effect.STEALTH_SHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.STEALTH_SHOT
end

xi.job_utils.ranger.useDoubleShot = function(player, target, ability, action)
    local p = xi.job_utils.ranger.doubleShotFromParams()
    player:addStatusEffect(xi.effect.DOUBLE_SHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
    })

    return xi.effect.DOUBLE_SHOT
end

xi.job_utils.ranger.useBountyShot = function(player, target, ability, action)
    local mobTHLevel        = target:getTHlevel()
    local bountyShotTHLevel = 2 + player:getMod(xi.mod.BOUNTY_SHOT_TH_BONUS)
    local playerTHLevel     = player:getMod(xi.mod.TREASURE_HUNTER)
    local newTHLevel        = 0

    -- base animation was for gun, -1 = archery
    -- Note: hume male's archery animation is bugged and looks like shadowbind
    local bountyAnim = xi.job_utils.ranger.bountyShotAnimOffsetFromParams(
        player:getWeaponSkillType(xi.slot.RANGED)
    )
    if bountyAnim ~= 0 then
        action:setAnimation(target:getID(), action:getAnimation(target:getID()) + bountyAnim)
    end

    player:removeAmmo(1) -- TODO: does this check recycle?

    action:info(target:getID(), 1) -- Bounty shot sets the first bit likely for animation purposes
    ability:setMsg(xi.msg.basic.JA_NO_EFFECT_2)

    target:updateClaim(player)

    -- pre-apply up to max value of TH4
    if mobTHLevel < 4 and playerTHLevel > mobTHLevel then
        newTHLevel = math.min(4, playerTHLevel)

        target:setTHlevel(newTHLevel)

        mobTHLevel = newTHLevel
    end

    -- 100% success rate if bounty shot level is higher than their TH level
    if bountyShotTHLevel > mobTHLevel then
        ability:setMsg(xi.msg.basic.JA_TH_EFFECTIVENESS)
        target:setTHlevel(bountyShotTHLevel)

        return bountyShotTHLevel
    end

    -- https://www.bg-wiki.com/ffxi/Bounty_Shot
    -- https://wiki.ffo.jp/html/22203.html
    if mobTHLevel < 12 + player:getMod(xi.mod.TREASURE_HUNTER_CAP) then
        local treausureHunterLevelDiff = mobTHLevel - bountyShotTHLevel

        -- TODO: this rate is the same as THF treasure hunter procs. It is unclear if this has the same rate or better than THF auto attacks.
        -- This also assumes proc rate bonus works on Bounty Shot, but without mountains of data I wouldn't be able to tell.
        -- JP wiki implies these rates and functionality is the same as THF, but there's no data.
        -- BG wiki claims proc rates are similar to SA + TA procs, which seems likely given the 1 min timer on bounty shot.
        local procRate      = 0.10 / math.pow(2, treausureHunterLevelDiff)
        local procRateBonus = 1.0 + (target:getMod(xi.mod.TREASURE_HUNTER_PROC) + player:getMod(xi.mod.TREASURE_HUNTER_PROC)) / 100

        if math.random() < procRate * procRateBonus then
            newTHLevel = mobTHLevel + 1

            ability:setMsg(xi.msg.basic.JA_TH_EFFECTIVENESS)

            target:setTHlevel(newTHLevel)

            return newTHLevel
        end
    end

    -- If we got here, TH was upgraded to 3 or 4 from gear
    -- JP wiki indicates this doesn't happen, but printing incorrectly that the action didn't boost TH level seems weird
    if newTHLevel > 0 then
        ability:setMsg(xi.msg.basic.JA_TH_EFFECTIVENESS)

        return newTHLevel
    end

    return 0
end

xi.job_utils.ranger.useDecoyShot = function(player, target, ability, action)
    local p = xi.job_utils.ranger.decoyShotFromParams()
    target:addStatusEffect(xi.effect.DECOY_SHOT, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
    })

    return xi.effect.DECOY_SHOT
end

xi.job_utils.ranger.useHoverShot = function(player, target, ability, action)
    return 0, 0 -- Not implemented yet
end

xi.job_utils.ranger.useOverkill = function(player, target, ability, action)
    local p = xi.job_utils.ranger.overkillFromParams()
    player:addStatusEffect(xi.effect.OVERKILL, {
        power    = p.power,
        duration = p.duration,
        origin   = player,
        tick     = p.tick,
    })

    return xi.effect.OVERKILL
end
