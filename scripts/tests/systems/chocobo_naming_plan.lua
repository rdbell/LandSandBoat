-----------------------------------
-- Pure system tests for chocobo naming pure plan (slice 6157).
-----------------------------------

describe('chocobo naming pure plan', function()
    local banned = {
        ['Blazing Uranus'] = true,
        ['Uranus Meteor'] = true,
    }

    -- sample name catalog subset for pure tests
    local names = {
        [0] = 'G',
        [12] = 'Ace',
        [100] = 'Chocobo', -- may not match real index; inject only
    }

    local function decode(option)
        return {
            offset1 = bit.band(0x3FF, bit.rshift(option, 8)),
            offset2 = bit.band(0x3FF, bit.rshift(option, 18)),
        }
    end

    local function fullKey(first, last)
        return string.format('%s %s', first, last)
    end

    local function nameTooLong(key)
        return string.len(key) > (15 + 1)
    end

    local function plan(option, nameTable, bannedSet)
        local d = decode(option)
        local fname = nameTable[d.offset1]
        local lname = nameTable[d.offset2]
        if not fname or not lname then
            return { ok = false, reason = 'lookup' }
        end
        local key = fullKey(fname, lname)
        if nameTooLong(key) then
            return { ok = false, reason = 'toolong', key = key }
        end
        if bannedSet[key] then
            return { ok = false, reason = 'banned', key = key }
        end
        local defaultChoco = (fname == 'Chocobo' and lname == 'Chocobo')
        return {
            ok = true,
            first = fname,
            last = lname,
            key = key,
            defaultChoco = defaultChoco,
            -- updateEvent all-1s when default Chocobo Chocobo
            eventAllOnes = defaultChoco,
        }
    end

    local function packOption(o1, o2)
        return bit.lshift(o1, 8) + bit.lshift(o2, 18)
    end

    it('decodes name offsets', function()
        local d = decode(packOption(12, 100))
        assert(d.offset1 == 12 and d.offset2 == 100)
        d = decode(packOption(0x3FF, 0x3FF))
        assert(d.offset1 == 0x3FF and d.offset2 == 0x3FF)
    end)

    it('full key and length gate', function()
        assert(fullKey('Ace', 'G') == 'Ace G')
        assert(nameTooLong('1234567890123456') == false) -- 16 not greater
        assert(nameTooLong('12345678901234567') == true) -- 17
        assert(nameTooLong(fullKey('AAAAAAAAAAAAAAA', 'B')) == true) -- 15+1+1=17
    end)

    it('banned names', function()
        assert(banned['Blazing Uranus'] and banned['Uranus Meteor'])
        assert(not banned['Ace G'])
    end)

    it('plan lookup fail and success', function()
        local r = plan(packOption(999, 0), names, banned)
        assert(r.ok == false and r.reason == 'lookup')
        r = plan(packOption(12, 0), names, banned)
        assert(r.ok and r.first == 'Ace' and r.last == 'G' and not r.eventAllOnes)
        r = plan(packOption(100, 100), names, banned)
        assert(r.ok and r.defaultChoco and r.eventAllOnes)
    end)

    it('plan rejects banned', function()
        local tab = { [1] = 'Blazing', [2] = 'Uranus' }
        -- fullKey is 'Blazing Uranus'
        local r = plan(packOption(1, 2), tab, banned)
        assert(r.ok == false and r.reason == 'banned')
    end)
end)
