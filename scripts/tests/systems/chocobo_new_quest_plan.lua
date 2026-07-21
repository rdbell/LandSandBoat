-----------------------------------
-- Pure system tests for newChocobo + quest events + age (slice 6167).
-----------------------------------

describe('chocobo new state and quest events pure plan', function()
    local Y = 0
    local EGG = 1
    local daysToAdult4 = 129
    local dayLength = 86400

    local function planAge(created, now)
        local age = math.floor((now - created) / dayLength) + 1
        if age > daysToAdult4 + 1 then
            age = daysToAdult4 + 1
        end
        return age
    end

    local function planNew(p)
        -- p: sex, dna alleles, ability, location
        local care = 0x70707070
        return {
            first = 'Chocobo',
            last = 'Chocobo',
            sex = p.sex,
            age = 0,
            last_update_age = 1,
            stage = EGG,
            location = p.location,
            allele1 = p.dna[1],
            allele2 = p.dna[2],
            allele3 = p.dna[3],
            color = p.color,
            strength = 0,
            endurance = 0,
            discernment = 0,
            receptivity = 0,
            affection = 255,
            energy = 100,
            satisfaction = 0,
            conditions = 0,
            ability1 = p.ability,
            ability2 = 0,
            personality = 0,
            weather_preference = 0,
            hunger = 0,
            care_plan = care,
            held_item = 0,
        }
    end

    local function planQuest(p)
        -- returns list of { age, cutscene }
        local events = {}
        local qs = {
            whiteHandkerchiefStarted = p.started,
            whiteHandkerchiefCancelled = p.cancelled,
            whiteHandkerchiefFinished = p.finished,
            chocoboWhistleQuestBegan = p.whistleBegan,
        }
        if not qs.whiteHandkerchiefStarted and not p.hasWhiteKI and p.age == 7 and not qs.chocoboWhistleQuestBegan then
            events[#events + 1] = { 7, 69 } -- CRYING_AT_NIGHT
            qs.whiteHandkerchiefStarted = true
        elseif qs.whiteHandkerchiefStarted and not qs.whiteHandkerchiefCancelled and p.age == 15 and p.reportLength >= 7 then
            events[#events + 1] = { 15, 53 } -- HAVENT_SEEN_YOU
            qs.whiteHandkerchiefCancelled = true
        elseif not qs.whiteHandkerchiefStarted and not qs.whiteHandkerchiefCancelled and not qs.whiteHandkerchiefFinished and p.age >= 8 and p.hasWhiteKI then
            events[#events + 1] = { p.age, 54 } -- THAT_SHOULD_BE_ENOUGH
            qs.whiteHandkerchiefFinished = true
        end
        return events, qs
    end

    it('computes age with clamp', function()
        assert(planAge(0, 0) == 1)
        assert(planAge(0, dayLength) == 2)
        assert(planAge(0, dayLength * 200) == daysToAdult4 + 1)
    end)

    it('new chocobo defaults', function()
        local c = planNew({ sex = 0, dna = { Y, Y, Y }, color = Y, ability = 0, location = 1 })
        assert(c.first == 'Chocobo' and c.last == 'Chocobo')
        assert(c.stage == EGG and c.affection == 255 and c.energy == 100)
        assert(c.care_plan == 0x70707070 and c.held_item == 0)
        assert(c.strength == 0 and c.hunger == 0)
    end)

    it('white handkerchief quest branches', function()
        local ev, qs = planQuest({
            age = 7, hasWhiteKI = false, reportLength = 0,
            started = false, cancelled = false, finished = false, whistleBegan = false,
        })
        assert(#ev == 1 and ev[1][2] == 69 and qs.whiteHandkerchiefStarted)
        ev, qs = planQuest({
            age = 15, hasWhiteKI = false, reportLength = 7,
            started = true, cancelled = false, finished = false, whistleBegan = false,
        })
        assert(#ev == 1 and ev[1][2] == 53 and qs.whiteHandkerchiefCancelled)
        ev, qs = planQuest({
            age = 10, hasWhiteKI = true, reportLength = 0,
            started = false, cancelled = false, finished = false, whistleBegan = false,
        })
        assert(#ev == 1 and ev[1][2] == 54 and qs.whiteHandkerchiefFinished)
        -- no start if whistle began
        ev = planQuest({
            age = 7, hasWhiteKI = false, reportLength = 0,
            started = false, cancelled = false, finished = false, whistleBegan = true,
        })
        assert(#ev == 0)
    end)
end)
