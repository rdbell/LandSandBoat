-----------------------------------
-- Pure system tests for xi.ability.adjustDamage full plan (slice 6109).
-- Source: scripts/globals/ability.lua ~7–100
-----------------------------------

describe('ability adjustDamage full pure plan', function()
    local MSG_ANTICIPATE = 30
    local MSG_SHADOW_ABSORB = 31
    local MSG_JA_MISS = 158
    local MSG_USES_JA_TAKE_DAMAGE = 317
    local MSG_JA_MISS_2 = 324

    local ATTACK_PHYSICAL = 1
    local ATTACK_MAGICAL = 2
    local ATTACK_BREATH = 4

    local SHADOW_IGNORE = 0
    local SHADOW_NUM2 = 2
    local SHADOW_WIPE = 999

    local function isPriorMiss(msg)
        return msg == MSG_JA_MISS or msg == 188 or msg == MSG_SHADOW_ABSORB or msg == MSG_ANTICIPATE
    end

    local function adjustEarly(p)
        if isPriorMiss(p.skillMsg or 0) then
            return { early = true, damage = 0, setMsg = false }
        end
        if p.skillType == ATTACK_PHYSICAL and (p.hasPerfectDodge or p.hasAllMiss) then
            return { early = true, damage = 0, setMsg = true, msg = MSG_JA_MISS_2 }
        end
        local dmg = p.dmg or 0
        local res = { early = false, damage = dmg, setMsg = true, msg = MSG_USES_JA_TAKE_DAMAGE }
        local sb = p.shadowBehavior or SHADOW_IGNORE
        if sb ~= SHADOW_WIPE and sb ~= SHADOW_IGNORE then
            dmg = p.damageAfterTakeShadows or 0
            if dmg == 0 then
                return { early = true, damage = sb, setMsg = true, msg = MSG_SHADOW_ABSORB }
            end
            res.damage = dmg
            return res
        end
        if sb == SHADOW_WIPE then
            res.wipeShadows = true
        end
        return res
    end

    local function adjustLate(p)
        if (p.skillType == ATTACK_PHYSICAL or p.skillType == 3) and p.anticipated then
            return { damage = 0, setMsg = true, msg = MSG_ANTICIPATE }
        end
        local dmg = p.dmg or 0
        if p.skillType == ATTACK_MAGICAL then
            dmg = math.floor(dmg * (p.magicAdj or 1))
            dmg = math.floor(dmg * (p.absorption or 1))
            dmg = math.floor(dmg * (p.nullification or 1))
        elseif p.skillType == ATTACK_BREATH then
            dmg = math.floor(dmg * (p.breathAdj or 1))
            dmg = math.floor(dmg * (p.absorption or 1))
            dmg = math.floor(dmg * (p.nullification or 1))
        end
        if dmg < 0 then
            return { damage = dmg }
        end
        if p.usePhalanx then dmg = p.phalanx end
        if p.skillType == ATTACK_MAGICAL and p.useOneForAll then dmg = p.oneForAll end
        if p.useStoneskin then dmg = p.stoneskin end
        local r = { damage = dmg }
        if dmg > 0 then
            r.wakeUp = true
            r.updateEnmity = true
        end
        return r
    end

    local function adjust(p)
        local early = adjustEarly(p)
        if early.early then
            return {
                damage = early.damage, setMsg = early.setMsg, msg = early.msg,
                wipeShadows = early.wipeShadows, earlyOut = true,
            }
        end
        p.dmg = early.damage
        local late = adjustLate(p)
        local r = {
            damage = late.damage, wipeShadows = early.wipeShadows,
            wakeUp = late.wakeUp, updateEnmity = late.updateEnmity, earlyOut = false,
        }
        if late.setMsg then
            r.setMsg = true
            r.msg = late.msg
        elseif early.setMsg then
            r.setMsg = true
            r.msg = early.msg
        end
        return r
    end

    it('prior miss early-out', function()
        local r = adjust({ dmg = 100, skillMsg = MSG_JA_MISS, skillType = ATTACK_BREATH })
        assert(r.earlyOut == true and r.damage == 0 and r.setMsg ~= true)
    end)

    it('perfect dodge physical', function()
        local r = adjust({
            dmg = 100, skillType = ATTACK_PHYSICAL, hasPerfectDodge = true,
            shadowBehavior = SHADOW_IGNORE,
        })
        assert(r.earlyOut == true and r.msg == MSG_JA_MISS_2)
    end)

    it('breath ignore shadows through mitigation', function()
        local r = adjust({
            dmg = 200, skillType = ATTACK_BREATH, shadowBehavior = SHADOW_IGNORE,
            breathAdj = 1, absorption = 1, nullification = 1,
            usePhalanx = true, phalanx = 180,
            useStoneskin = true, stoneskin = 150,
        })
        assert(r.earlyOut == false and r.damage == 150)
        assert(r.msg == MSG_USES_JA_TAKE_DAMAGE and r.wakeUp == true)
    end)

    it('shadow full absorb returns count', function()
        local r = adjust({
            dmg = 100, skillType = ATTACK_PHYSICAL,
            shadowBehavior = SHADOW_NUM2, damageAfterTakeShadows = 0,
        })
        assert(r.earlyOut == true and r.damage == SHADOW_NUM2 and r.msg == MSG_SHADOW_ABSORB)
    end)

    it('anticipate rewrites message', function()
        local r = adjust({
            dmg = 100, skillType = ATTACK_PHYSICAL, shadowBehavior = SHADOW_IGNORE,
            anticipated = true,
        })
        assert(r.damage == 0 and r.msg == MSG_ANTICIPATE)
    end)

    it('wipe shadows flag continues', function()
        local r = adjust({
            dmg = 50, skillType = ATTACK_BREATH, shadowBehavior = SHADOW_WIPE,
            breathAdj = 1, absorption = 1, nullification = 1,
        })
        assert(r.wipeShadows == true and r.damage == 50)
    end)

    it('magical absorb negative no wake', function()
        local r = adjust({
            dmg = 100, skillType = ATTACK_MAGICAL, shadowBehavior = SHADOW_IGNORE,
            magicAdj = 1, absorption = -1, nullification = 1,
        })
        assert(r.damage < 0 and r.wakeUp ~= true)
    end)
end)
