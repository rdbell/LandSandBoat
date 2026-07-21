-----------------------------------
-- Pure system tests for useEnhancingSpell full plan (slice 6113).
-- Source: scripts/globals/spells/enhancing_spell.lua ~481–614
-----------------------------------

describe('useEnhancingSpell pure plan', function()
    local EFFECT_NONE = 255
    local EFFECT_EMBRAVA = 228
    local EFFECT_REFRESH = 43
    local EFFECT_INVISIBLE = 69
    local EFFECT_BARFIRE = 100
    local EFFECT_DREAD_SPIKES = 266
    local EFFECT_STONESKIN = 37
    local EFFECT_STR_BOOST = 80
    local EFFECT_FIRESTORM = 178
    local EFFECT_PROTECT = 40

    local MSG_MAGIC_CANNOT_CAST = 47
    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_GAIN_EFFECT = 230

    local SPELL_GROUP_WHITE = 6 -- xi.magic.spellGroup.WHITE

    local function use(p)
        local effect = p.spellEffect or 0
        local subPower = 0

        if effect >= EFFECT_BARFIRE and effect <= 105 then
            subPower = p.barSubPower or 0
        elseif effect == EFFECT_DREAD_SPIKES then
            subPower = p.dreadSubPower or 0
        elseif effect == EFFECT_EMBRAVA then
            if not p.hasTabulaRasa then
                return { returnEffect = 0, msg = MSG_MAGIC_CANNOT_CAST, cancelled = true }
            end
        elseif effect == EFFECT_REFRESH then
            if (p.tier or 0) < 3 and p.hasSublimation then
                return { returnEffect = 0, msg = MSG_MAGIC_NO_EFFECT, cancelled = true }
            end
        elseif effect == EFFECT_INVISIBLE then
            if p.hasInvisibleFlag then
                return { returnEffect = 0, msg = MSG_MAGIC_NO_EFFECT, cancelled = true }
            end
        end

        local r = {
            returnEffect = effect,
            subPower = subPower,
            power = p.finalPower or 0,
            duration = p.duration or 0,
            applyEffect = true,
            alwaysOverwrite = p.alwaysOverwrite == true,
            deleteBoostStats = (effect >= EFFECT_STR_BOOST and effect <= 86 and p.deleteBoostStats) or false,
            deleteStorms = (effect >= EFFECT_FIRESTORM and effect <= 185 and p.deleteStorms) or false,
        }

        if not p.casterIsPet and p.targetHasEmbolden and p.spellGroup == SPELL_GROUP_WHITE then
            r.deleteEmbolden = true
        end

        if p.alwaysOverwrite then
            return r
        end

        if p.addStatusOK then
            r.msg = MSG_MAGIC_GAIN_EFFECT
            return r
        end

        r.applyEffect = false
        r.msg = MSG_MAGIC_NO_EFFECT
        r.returnEffect = EFFECT_NONE
        return r
    end

    it('embrava without tabula cancels', function()
        local r = use({ spellEffect = EFFECT_EMBRAVA, hasTabulaRasa = false })
        assert(r.cancelled == true and r.msg == MSG_MAGIC_CANNOT_CAST)
    end)

    it('refresh under sublimation blocked for low tier', function()
        local r = use({ spellEffect = EFFECT_REFRESH, tier = 2, hasSublimation = true })
        assert(r.cancelled == true and r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('invisible flag blocks', function()
        local r = use({ spellEffect = EFFECT_INVISIBLE, hasInvisibleFlag = true })
        assert(r.cancelled == true)
    end)

    it('bar subpower and always overwrite', function()
        local r = use({
            spellEffect = EFFECT_BARFIRE, barSubPower = 15,
            finalPower = 100, duration = 480, alwaysOverwrite = true,
        })
        assert(r.subPower == 15 and r.alwaysOverwrite == true and r.applyEffect == true)
    end)

    it('gain effect message on success', function()
        local r = use({
            spellEffect = EFFECT_STONESKIN, finalPower = 200, duration = 300, addStatusOK = true,
        })
        assert(r.msg == MSG_MAGIC_GAIN_EFFECT and r.returnEffect == EFFECT_STONESKIN)
    end)

    it('add fail returns NONE', function()
        local r = use({ spellEffect = EFFECT_STONESKIN, finalPower = 200, addStatusOK = false })
        assert(r.applyEffect ~= true and r.returnEffect == EFFECT_NONE and r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('embolden delete for white non-pet', function()
        local r = use({
            spellEffect = EFFECT_PROTECT, spellGroup = SPELL_GROUP_WHITE,
            targetHasEmbolden = true, casterIsPet = false, alwaysOverwrite = true,
        })
        assert(r.deleteEmbolden == true)
    end)

    it('boost and storm delete flags', function()
        local r = use({
            spellEffect = EFFECT_STR_BOOST, deleteBoostStats = true, addStatusOK = true, finalPower = 10,
        })
        assert(r.deleteBoostStats == true)
        r = use({
            spellEffect = EFFECT_FIRESTORM, deleteStorms = true, alwaysOverwrite = true, finalPower = 2,
        })
        assert(r.deleteStorms == true)
    end)
end)
