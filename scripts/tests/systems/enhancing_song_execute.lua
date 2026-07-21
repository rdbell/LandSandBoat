-----------------------------------
-- Pure system tests for useEnhancingSong full plan (slice 6115).
-- Source: scripts/globals/spells/enhancing_song.lua ~266–311
-----------------------------------

describe('useEnhancingSong pure plan', function()
    local EFFECT_ETUDE = 215
    local EFFECT_CAROL = 216
    local EFFECT_MARCH = 214
    local EFFECT_MINUET = 198
    local EFFECT_BALLAD = 196

    local MSG_MAGIC_NO_EFFECT = 75

    local function resolveSubEffect(songEffect, tableSub, augmentSongStat, subModValue)
        if songEffect == EFFECT_CAROL then
            return (tableSub or 0) + ((augmentSongStat or 0) * 100)
        end
        if songEffect == EFFECT_ETUDE then
            return tableSub or 0
        end
        return subModValue or 0
    end

    local function marchPower(power)
        return math.floor((power / 1024) * 10000)
    end

    local function use(p)
        local songEffect = p.songEffect or 0
        local tier = p.tier or 1
        local power = p.power or 0
        local duration = p.duration or 0
        local paramFour = 0

        local subEffect = resolveSubEffect(
            songEffect,
            p.tableSub or 0,
            p.augmentSongStat or 0,
            p.subModValue or 0
        )

        if songEffect == EFFECT_ETUDE and tier == 2 then
            paramFour = 10
        end

        if songEffect == EFFECT_MARCH then
            power = marchPower(power)
        end

        local consumeMarcato = p.hasMarcato == true

        if not p.addBardSongOK then
            return {
                returnEffect = songEffect,
                msg = MSG_MAGIC_NO_EFFECT,
                setMsg = true,
                power = power,
                duration = duration,
                paramFour = paramFour,
                subEffect = subEffect,
                tier = tier,
                consumeMarcato = consumeMarcato,
                applySong = false,
            }
        end

        return {
            returnEffect = songEffect,
            setMsg = false,
            power = power,
            duration = duration,
            paramFour = paramFour,
            subEffect = subEffect,
            tier = tier,
            consumeMarcato = consumeMarcato,
            applySong = true,
        }
    end

    it('minuet success returns effect and apply plan', function()
        local r = use({
            songEffect = EFFECT_MINUET, tier = 3, power = 40, duration = 180,
            tableSub = 1003, subModValue = 5, addBardSongOK = true,
        })
        assert(r.returnEffect == EFFECT_MINUET)
        assert(r.applySong == true)
        assert(r.setMsg ~= true)
        assert(r.power == 40 and r.duration == 180 and r.paramFour == 0)
        assert(r.subEffect == 5 and r.tier == 3)
        assert(r.consumeMarcato ~= true)
    end)

    it('addBardSong fail sets MAGIC_NO_EFFECT', function()
        local r = use({
            songEffect = EFFECT_BALLAD, tier = 1, power = 1, duration = 120,
            subModValue = 0, addBardSongOK = false,
        })
        assert(r.msg == MSG_MAGIC_NO_EFFECT and r.setMsg == true)
        assert(r.applySong == false)
        assert(r.returnEffect == EFFECT_BALLAD)
    end)

    it('carol subEffect is table sub + augment*100', function()
        local r = use({
            songEffect = EFFECT_CAROL, tier = 1, power = 30, duration = 120,
            tableSub = 1, -- fire element
            augmentSongStat = 2, addBardSongOK = true,
        })
        assert(r.subEffect == 1 + 2 * 100)
        assert(r.paramFour == 0)
    end)

    it('etude subEffect is table sub; tier2 paramFour is 10', function()
        local r = use({
            songEffect = EFFECT_ETUDE, tier = 1, power = 6, duration = 120,
            tableSub = 8, -- STR
            addBardSongOK = true,
        })
        assert(r.subEffect == 8 and r.paramFour == 0)

        r = use({
            songEffect = EFFECT_ETUDE, tier = 2, power = 14, duration = 120,
            tableSub = 9, addBardSongOK = true,
        })
        assert(r.subEffect == 9 and r.paramFour == 10)
    end)

    it('non carol/etude subEffect uses caster mod inject', function()
        local r = use({
            songEffect = EFFECT_MINUET, tier = 1, power = 10, duration = 120,
            tableSub = 1003, subModValue = 7, addBardSongOK = true,
        })
        assert(r.subEffect == 7)
    end)

    it('march converts power floor((p/1024)*10000)', function()
        -- power 102 → floor(102/1024*10000) = floor(996.093...) = 996
        local r = use({
            songEffect = EFFECT_MARCH, tier = 1, power = 102, duration = 150,
            subModValue = 0, addBardSongOK = true,
        })
        assert(r.power == 996)
        assert(r.returnEffect == EFFECT_MARCH)

        r = use({
            songEffect = EFFECT_MARCH, tier = 2, power = 0, duration = 120,
            addBardSongOK = true,
        })
        assert(r.power == 0)

        -- 1024 → floor(10000) = 10000
        r = use({
            songEffect = EFFECT_MARCH, tier = 1, power = 1024, duration = 120,
            addBardSongOK = true,
        })
        assert(r.power == 10000)
    end)

    it('hasMarcato signals consume', function()
        local r = use({
            songEffect = EFFECT_MINUET, power = 20, duration = 120,
            hasMarcato = true, addBardSongOK = true,
        })
        assert(r.consumeMarcato == true)

        r = use({
            songEffect = EFFECT_MINUET, power = 20, duration = 120,
            hasMarcato = false, addBardSongOK = false,
        })
        assert(r.consumeMarcato ~= true)
    end)

    it('etude tier2 fail still returns effect and paramFour', function()
        local r = use({
            songEffect = EFFECT_ETUDE, tier = 2, power = 15, duration = 200,
            tableSub = 8, addBardSongOK = false, hasMarcato = true,
        })
        assert(r.returnEffect == EFFECT_ETUDE)
        assert(r.paramFour == 10 and r.subEffect == 8)
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
        assert(r.consumeMarcato == true)
        assert(r.power == 15)
    end)
end)
