-----------------------------------
-- Pure system tests for remaining additional_effects procs (slice 6132).
-- Source: scripts/globals/additional_effects.lua ~419–599
-----------------------------------

describe('additionalEffect proc SELF_BUFF pure plan', function()
    local EFFECT_BLINK = 36
    local EFFECT_HASTE = 33
    local EFFECT_COPY_IMAGE = 66
    local MSG_ADD_EFFECT_SELFBUFF = 166

    local function proc(p)
        if p.addStatus == EFFECT_BLINK then
            if p.hasBlink or p.hasCopyImage then
                return { noOp = true }
            end
            return {
                msgID = MSG_ADD_EFFECT_SELFBUFF, msgParam = EFFECT_BLINK,
                applyEffect = true, power = p.power or 0, duration = p.duration or 0,
            }
        end
        if p.addStatus == EFFECT_HASTE then
            return {
                msgID = MSG_ADD_EFFECT_SELFBUFF, msgParam = EFFECT_HASTE,
                applyEffect = true, power = p.power or 0, duration = p.duration or 0,
            }
        end
        return { noOp = true }
    end

    it('blink blocked by existing shadows', function()
        assert(proc({ addStatus = EFFECT_BLINK, hasBlink = true }).noOp == true)
        assert(proc({ addStatus = EFFECT_BLINK, hasCopyImage = true }).noOp == true)
    end)

    it('blink applies when clear', function()
        local r = proc({ addStatus = EFFECT_BLINK, power = 3, duration = 60 })
        assert(r.applyEffect == true and r.msgParam == EFFECT_BLINK)
    end)

    it('haste always applies', function()
        local r = proc({ addStatus = EFFECT_HASTE, power = 1000, duration = 30 })
        assert(r.msgID == MSG_ADD_EFFECT_SELFBUFF and r.msgParam == EFFECT_HASTE)
    end)
end)

describe('additionalEffect proc DEATH pure plan', function()
    local MSG_ADD_EFFECT_STATUS = 160
    local EFFECT_KO = 0

    local function proc(p)
        if p.isNM or p.isUndead or p.deathResFail then
            return { noOp = true }
        end
        return { msgID = MSG_ADD_EFFECT_STATUS, msgParam = EFFECT_KO, setHP0 = true }
    end

    it('nm undead and deathres fail block', function()
        assert(proc({ isNM = true }).noOp == true)
        assert(proc({ isUndead = true }).noOp == true)
        assert(proc({ deathResFail = true }).noOp == true)
    end)

    it('success KO', function()
        local r = proc({})
        assert(r.msgID == MSG_ADD_EFFECT_STATUS and r.msgParam == EFFECT_KO and r.setHP0 == true)
    end)
end)

describe('additionalEffect composite drain select pure plan', function()
    local PROC_HP = 5
    local PROC_MP = 6
    local PROC_TP = 7

    local function select(includeTP, roll)
        if roll == 1 then return PROC_HP end
        if roll == 2 then return PROC_MP end
        if includeTP and roll == 3 then return PROC_TP end
        return 0
    end

    it('hpmp maps 1-2', function()
        assert(select(false, 1) == PROC_HP and select(false, 2) == PROC_MP)
        assert(select(false, 3) == 0)
    end)

    it('hpmptp maps 1-3', function()
        assert(select(true, 3) == PROC_TP)
    end)
end)

describe('additionalEffect NM_SPECIFIC match pure plan', function()
    local function match(required, item)
        return required == 0 or required == item
    end

    it('NONE required matches any', function()
        assert(match(0, 999) == true)
    end)

    it('required item must match', function()
        assert(match(100, 100) == true)
        assert(match(100, 99) == false)
    end)
end)
