-----------------------------------
-- Pure system tests for executeAddEffectDamage full plan (slice 6104).
-- Source: scripts/globals/combat/action_additional_effect_damage.lua ~101–200
-----------------------------------

describe('executeAddEffectDamage pure plan', function()
    local DAMAGE_TYPE_ELEMENTAL = 5
    local MSG_ADD_EFFECT_DMG = 163
    local MSG_ADD_EFFECT_HEAL = 384
    local MSG_ADD_EFFECT_HP_DRAIN = 161
    local MSG_ADD_EFFECT_MP_DRAIN = 162
    local MSG_ADD_EFFECT_TP_DRAIN = 165

    local function actionDamageType(physicalElement, magicalElement)
        if physicalElement > 0 then
            return physicalElement
        end
        return DAMAGE_TYPE_ELEMENTAL + magicalElement
    end

    local function clampDamage(damage)
        if damage < 0 then
            return 0
        end
        if damage > 99999 then
            return 99999
        end
        return damage
    end

    local function handlePhalanx(damage, phalanxMod)
        if damage <= 0 then
            return damage
        end
        local d = damage - phalanxMod
        if d < 0 then
            return 0
        end
        return d
    end

    local function handleOneForAll(damage, power, hasEffect)
        if not hasEffect or damage <= 0 then
            return damage
        end
        local d = damage - power
        if d < 0 then
            return 0
        end
        return d
    end

    local function handleStoneskin(damage, skin)
        if damage <= 0 or skin <= 0 then
            return damage
        end
        if skin >= damage then
            return 0
        end
        return damage - skin
    end

    local function applyPositiveMitigation(damage, phalanxMod, ofaPower, stoneskin, ofaActive)
        if damage <= 0 then
            return damage
        end
        damage = clampDamage(handlePhalanx(damage, phalanxMod))
        damage = clampDamage(handleOneForAll(damage, ofaPower, ofaActive))
        damage = clampDamage(handleStoneskin(damage, stoneskin))
        return damage
    end

    local function drainClamp(damage, resource, overDrain)
        if overDrain then
            return damage
        end
        if damage < 0 then
            return 0
        end
        if damage > resource then
            return resource
        end
        return damage
    end

    -- Simplified product: base only with identity mults, gates as bools.
    local function product(p)
        if p.hasEnspell or p.procMiss or p.limitUndead or p.resistBelowFloor then
            return 0
        end
        local damage = p.base or 0
        local mults = p.mults or { 1 }
        for i = 1, #mults do
            damage = math.floor(damage * mults[i])
        end
        return damage
    end

    local function execute(p)
        local msgDmg = p.messageDamage or MSG_ADD_EFFECT_DMG
        local msgHeal = p.messageHeal or MSG_ADD_EFFECT_HEAL
        local damage = product(p)
        damage = applyPositiveMitigation(
            damage,
            p.phalanxMod or 0,
            p.ofaPower or 0,
            p.stoneskin or 0,
            p.ofaActive == true
        )

        local actorAddHP, actorAddMP, actorAddTP = 0, 0, 0
        if p.drainHP then
            damage = drainClamp(damage, p.targetHP or 0, p.overDrain == true)
            msgDmg = MSG_ADD_EFFECT_HP_DRAIN
            if damage > 0 then
                actorAddHP = damage
            end
        end
        if p.drainMP then
            damage = drainClamp(damage, p.targetMP or 0, p.overDrain == true)
            msgDmg = MSG_ADD_EFFECT_MP_DRAIN
            if damage > 0 then
                actorAddMP = damage
            end
        end
        if p.drainTP then
            damage = drainClamp(damage, p.targetTP or 0, p.overDrain == true)
            msgDmg = MSG_ADD_EFFECT_TP_DRAIN
            if damage > 0 then
                actorAddTP = damage
            end
        end

        if damage == 0 then
            return { proc = false, animation = 0, message = 0, amount = 0 }
        end
        if damage < 0 then
            return {
                proc = true,
                animation = p.animation or 0,
                message = msgHeal,
                amount = -damage,
                targetHealHP = -damage,
            }
        end
        return {
            proc = true,
            animation = p.animation or 0,
            message = msgDmg,
            amount = damage,
            takeDamage = true,
            actionDamageType = actionDamageType(p.physicalElement or 0, p.magicalElement or 0),
            actorAddHP = actorAddHP,
            actorAddMP = actorAddMP,
            actorAddTP = actorAddTP,
        }
    end

    it('actionDamageType prefers physical', function()
        assert(actionDamageType(2, 1) == 2)
        assert(actionDamageType(0, 1) == DAMAGE_TYPE_ELEMENTAL + 1)
    end)

    it('enspell gate yields no proc', function()
        local r = execute({ hasEnspell = true, base = 100 })
        assert(r.proc == false and r.amount == 0)
    end)

    it('damage path returns animation message amount', function()
        local r = execute({ base = 100, animation = 1, physicalElement = 2 })
        assert(r.proc == true and r.amount == 100 and r.message == MSG_ADD_EFFECT_DMG)
        assert(r.takeDamage == true and r.actionDamageType == 2)
    end)

    it('absorb mult heal path', function()
        local r = execute({ base = 50, mults = { -1 }, animation = 7 })
        assert(r.proc == true and r.amount == 50 and r.message == MSG_ADD_EFFECT_HEAL)
        assert(r.targetHealHP == 50)
    end)

    it('HP drain clamps to target HP and sets message', function()
        local r = execute({ base = 100, drainHP = true, targetHP = 30 })
        assert(r.amount == 30 and r.message == MSG_ADD_EFFECT_HP_DRAIN)
        assert(r.actorAddHP == 30)
    end)

    it('last drain wins message (TP after HP)', function()
        local r = execute({
            base = 50, drainHP = true, drainTP = true,
            targetHP = 1000, targetTP = 1000, overDrain = true,
        })
        assert(r.message == MSG_ADD_EFFECT_TP_DRAIN)
        assert(r.actorAddHP == 50 and r.actorAddTP == 50)
    end)

    it('full phalanx mitigation yields no proc', function()
        local r = execute({ base = 50, phalanxMod = 1000 })
        assert(r.proc == false)
    end)

    it('mitigation chain phalanx ofa stoneskin', function()
        -- 200-30=170, ofa 40 → 130, stoneskin 50 → 80
        local r = execute({
            base = 200, phalanxMod = 30, ofaPower = 40, ofaActive = true, stoneskin = 50,
        })
        assert(r.amount == 80)
    end)
end)
