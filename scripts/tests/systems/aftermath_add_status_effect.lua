-----------------------------------
-- Pure system tests for aftermath.addStatusEffect plan (slice 6134).
-- Source: scripts/globals/aftermath.lua ~554–622
-----------------------------------

describe('aftermath addStatusEffect pure plan gates', function()
    local TYPE_RELIC = 1
    local TYPE_MYTHIC = 2
    local TYPE_EMPY = 3

    local function validID(typ, id)
        if typ == TYPE_RELIC then return id >= 1 and id <= 28 end
        if typ == TYPE_MYTHIC then return id >= 29 and id <= 43 end
        if typ == TYPE_EMPY then return id >= 44 and id <= 45 end
        return false
    end

    local function relicDuration(id, tp)
        if id >= 1 and id <= 14 then return math.floor(tp * 0.02) end
        if id >= 15 and id <= 28 then return math.floor(tp * 0.06) end
        return nil
    end

    local mythicDur = {
        [29] = { [1] = 60, [2] = 90, [3] = 120 },
    }
    local empyDur = {
        [44] = { [1] = 30, [2] = 60, [3] = 90 },
        [45] = { [1] = 60, [2] = 120, [3] = 180 },
    }

    local function duration(typ, id, tp)
        local lvl = math.floor(tp / 1000)
        if typ == TYPE_RELIC then return relicDuration(id, tp) end
        if typ == TYPE_MYTHIC then
            local row = mythicDur[id]
            if not row then return nil end
            return row[lvl]
        end
        if typ == TYPE_EMPY then
            local row = empyDur[id]
            if not row then return nil end
            return row[lvl]
        end
        return nil
    end

    local function canOverwrite(p)
        if not p.hasEffect then return true end
        if p.newType < p.existingTier then return false end
        if p.newType == TYPE_RELIC then
            local secs = duration(TYPE_RELIC, p.newID, p.newTP)
            if not secs then return false end
            return secs * 1000 > p.existingTimeRemainingMs
        end
        if p.newType == TYPE_MYTHIC or p.newType == TYPE_EMPY then
            local cur = math.floor(p.existingSubPower / 1000)
            local neu = math.floor(p.newTP / 1000)
            return cur == 1 or cur < neu
        end
        return false
    end

    local function plan(p)
        if not p.isPC or not p.hasWeapon then return { ok = false } end
        if not validID(p.typ, p.id) then return { ok = false } end
        local dur = duration(p.typ, p.id, p.tp)
        if not dur then return { ok = false } end
        if not canOverwrite({
            hasEffect = p.hasEffect, existingTier = p.existingTier or 0,
            existingSubPower = p.existingSubPower or 0,
            existingTimeRemainingMs = p.existingTimeRemainingMs or 0,
            newType = p.typ, newID = p.id, newTP = p.tp,
        }) then
            return { ok = false }
        end
        local r = {
            ok = true, delAftermath = true, power = p.id, duration = dur,
            subPower = p.tp, tier = p.typ, clearSpikes = false,
        }
        if p.typ == TYPE_RELIC and (p.id == 8 or p.id == 22) then
            r.clearSpikes = true
        end
        if p.typ == TYPE_MYTHIC or p.typ == TYPE_EMPY then
            r.hasIcon = true
            r.level = math.floor(p.tp / 1000)
        end
        return r
    end

    it('blocks non-pc and missing weapon', function()
        assert(plan({ isPC = false, hasWeapon = true, id = 1, typ = TYPE_RELIC, tp = 1000 }).ok == false)
        assert(plan({ isPC = true, hasWeapon = false, id = 1, typ = TYPE_RELIC, tp = 1000 }).ok == false)
    end)

    it('blocks invalid id for type', function()
        assert(plan({ isPC = true, hasWeapon = true, id = 29, typ = TYPE_RELIC, tp = 1000 }).ok == false)
        assert(plan({ isPC = true, hasWeapon = true, id = 1, typ = TYPE_MYTHIC, tp = 1000 }).ok == false)
    end)

    it('applies relic with duration and optional spikes clear', function()
        local r = plan({ isPC = true, hasWeapon = true, id = 1, typ = TYPE_RELIC, tp = 2000 })
        assert(r.ok == true and r.duration == 40 and r.clearSpikes == false)
        local g = plan({ isPC = true, hasWeapon = true, id = 8, typ = TYPE_RELIC, tp = 1000 })
        assert(g.ok == true and g.clearSpikes == true and g.duration == 20)
    end)

    it('applies mythic with level icon', function()
        local r = plan({ isPC = true, hasWeapon = true, id = 29, typ = TYPE_MYTHIC, tp = 2500 })
        assert(r.ok == true and r.duration == 90 and r.hasIcon == true and r.level == 2)
    end)

    it('respects overwrite policy', function()
        local blocked = plan({
            isPC = true, hasWeapon = true, id = 1, typ = TYPE_RELIC, tp = 1000,
            hasEffect = true, existingTier = TYPE_RELIC, existingTimeRemainingMs = 30000,
        })
        assert(blocked.ok == false)
        local ok = plan({
            isPC = true, hasWeapon = true, id = 1, typ = TYPE_RELIC, tp = 1000,
            hasEffect = true, existingTier = TYPE_RELIC, existingTimeRemainingMs = 10000,
        })
        assert(ok.ok == true)
    end)
end)
