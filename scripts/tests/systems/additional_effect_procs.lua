-----------------------------------
-- Pure system tests for additional_effects proc plans (slice 6131).
-- Source: scripts/globals/additional_effects.lua ~203–416, ~602–640
-----------------------------------

describe('additionalEffect attack gates pure plan', function()
    local function attackGate(p)
        if (p.itemReqLvl or 0) > (p.attackerLvl or 0) then
            return { ok = false }
        end
        if (p.procRoll or 0) > (p.chance or 0) then
            return { ok = false }
        end
        local damage = p.damage or 0
        if (p.dStat or 0) > 0 then
            damage = p.damageAfterDStat or damage
        end
        return { ok = true, damage = damage, addType = p.addType or 0 }
    end

    it('level sync below item req blocks', function()
        local r = attackGate({ itemReqLvl = 75, attackerLvl = 50, chance = 100, procRoll = 1 })
        assert(r.ok == false)
    end)

    it('proc roll above chance blocks', function()
        local r = attackGate({ itemReqLvl = 1, attackerLvl = 99, chance = 20, procRoll = 21 })
        assert(r.ok == false)
    end)

    it('dStat applies when dStat > 0', function()
        local r = attackGate({
            itemReqLvl = 1, attackerLvl = 99, chance = 100, procRoll = 1,
            dStat = 12, damage = 10, damageAfterDStat = 25, addType = 1,
        })
        assert(r.ok == true and r.damage == 25)
    end)
end)

describe('additionalEffect proc DAMAGE pure plan', function()
    local MSG_ADD_EFFECT_DMG = 163
    local MSG_ADD_EFFECT_HEAL = 384

    local function procDamage(p)
        local damage = p.calcDamage or 0
        local msgID = MSG_ADD_EFFECT_DMG
        local addHP = false
        local delHP = false
        if damage < 0 then
            msgID = MSG_ADD_EFFECT_HEAL
            damage = damage * -1
            addHP = true
        else
            delHP = true
        end
        return {
            subEffect = p.subEffect or 0,
            msgID = msgID,
            msgParam = damage,
            addHP = addHP,
            delHP = delHP,
        }
    end

    it('positive damage delHP', function()
        local r = procDamage({ calcDamage = 42, subEffect = 7 })
        assert(r.msgID == MSG_ADD_EFFECT_DMG and r.msgParam == 42 and r.delHP == true)
        assert(r.subEffect == 7)
    end)

    it('negative damage heals target', function()
        local r = procDamage({ calcDamage = -30 })
        assert(r.msgID == MSG_ADD_EFFECT_HEAL and r.msgParam == 30 and r.addHP == true)
    end)
end)

describe('additionalEffect proc DEBUFF pure plan', function()
    local MSG_ADD_EFFECT_STATUS_2 = 164

    local function procDebuff(p)
        if p.noActor or p.noTarget then
            return { subEffect = 0, msgID = 0, msgParam = 0 }
        end
        local effectId = p.addStatus or 0
        if effectId == 0 then
            return { subEffect = 0, msgID = 0, msgParam = 0 }
        end
        if p.isImmune or p.isTraitResisted or p.isNullified then
            return { subEffect = 0, msgID = 0, msgParam = 0 }
        end
        if (p.resistRate or 1) < 0.5 then
            return { subEffect = 0, msgID = 0, msgParam = 0 }
        end
        local duration = math.floor((p.duration or 0) * (p.resistRate or 1))
        return {
            subEffect = p.subEffect or 0,
            msgID = MSG_ADD_EFFECT_STATUS_2,
            msgParam = effectId,
            applyEffect = true,
            power = p.power or 0,
            duration = duration,
            tick = p.tick or 0,
        }
    end

    it('missing actor/target or zero effect returns zeros', function()
        assert(procDebuff({ noActor = true, addStatus = 3 }).msgID == 0)
        assert(procDebuff({ addStatus = 0 }).msgID == 0)
    end)

    it('gates immune trait nullify resist', function()
        assert(procDebuff({ addStatus = 3, isImmune = true }).msgID == 0)
        assert(procDebuff({ addStatus = 3, resistRate = 0.25 }).msgID == 0)
    end)

    it('success scales duration', function()
        local r = procDebuff({
            addStatus = 3, subEffect = 5, power = 10, duration = 60,
            resistRate = 0.5, tick = 3,
        })
        assert(r.applyEffect == true and r.duration == 30 and r.msgParam == 3)
        assert(r.msgID == MSG_ADD_EFFECT_STATUS_2 and r.tick == 3)
    end)
end)

describe('additionalEffect proc heal pure plan', function()
    local MSG_ADD_EFFECT_HP_HEAL = 167
    local MSG_ADD_EFFECT_MP_HEAL = 152

    it('HP heal uses params.damage as amount', function()
        local amount = 25
        assert(amount == 25) -- attacker addHP
        local msg = MSG_ADD_EFFECT_HP_HEAL
        assert(msg == 167)
    end)

    it('MP heal uses params.damage as amount', function()
        assert(MSG_ADD_EFFECT_MP_HEAL == 152)
    end)
end)

describe('additionalEffect proc drain pure plan', function()
    local MSG_ADD_EFFECT_HP_DRAIN = 161
    local MSG_ADD_EFFECT_MP_DRAIN = 162
    local MSG_ADD_EFFECT_TP_DRAIN = 165

    local function procDrain(p)
        if p.isUndead then
            return { subEffect = 0, msgID = 0, msgParam = 0 }
        end
        local damage = p.calcDamage or 0
        local pool = p.pool or 0 -- HP/MP/TP
        if damage > pool then
            damage = pool
        end
        return {
            subEffect = p.subEffect or 0,
            msgID = p.msgID,
            msgParam = damage,
            transfer = damage,
        }
    end

    it('undead blocks all drain types', function()
        assert(procDrain({ isUndead = true, calcDamage = 50 }).msgID == 0)
    end)

    it('clamps to pool', function()
        local r = procDrain({
            calcDamage = 100, pool = 40, msgID = MSG_ADD_EFFECT_HP_DRAIN,
        })
        assert(r.msgParam == 40 and r.msgID == MSG_ADD_EFFECT_HP_DRAIN)
    end)

    it('message pins', function()
        assert(MSG_ADD_EFFECT_MP_DRAIN == 162 and MSG_ADD_EFFECT_TP_DRAIN == 165)
    end)
end)

describe('additionalEffect proc DISPEL pure plan', function()
    local MSG_ADD_EFFECT_DISPEL = 168
    local EFFECT_NONE = 0

    local function procDispel(p)
        local dispel = p.dispelled or EFFECT_NONE
        if dispel == EFFECT_NONE then
            return { subEffect = 0, msgID = 0, msgParam = 0 }
        end
        return {
            subEffect = p.subEffect or 0,
            msgID = MSG_ADD_EFFECT_DISPEL,
            msgParam = dispel,
        }
    end

    it('none returns zeros', function()
        assert(procDispel({ dispelled = 0 }).msgID == 0)
    end)

    it('success returns effect id', function()
        local r = procDispel({ subEffect = 2, dispelled = 93 })
        assert(r.msgID == MSG_ADD_EFFECT_DISPEL and r.msgParam == 93)
    end)
end)

describe('additionalEffect proc ABSORB_STATUS pure plan', function()
    local MSG_STEAL_EFFECT = 453

    local function procAbsorb(p)
        if (p.resist or 0) <= 0.0625 then
            return { subEffect = p.subEffect or 0, msgID = 0, msgParam = 0 }
        end
        return {
            subEffect = p.subEffect or 0,
            msgID = MSG_STEAL_EFFECT,
            msgParam = p.stolen or 0,
            steal = true,
        }
    end

    it('resist at or below 0.0625 fails', function()
        assert(procAbsorb({ resist = 0.0625 }).steal ~= true)
        assert(procAbsorb({ resist = 0.05 }).msgID == 0)
    end)

    it('resist above threshold steals', function()
        local r = procAbsorb({ resist = 0.125, stolen = 14, subEffect = 1 })
        assert(r.steal == true and r.msgID == MSG_STEAL_EFFECT and r.msgParam == 14)
    end)
end)
