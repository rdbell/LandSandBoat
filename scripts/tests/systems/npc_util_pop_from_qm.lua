-----------------------------------
-- Pure system tests for popFromQM pure plans (slice 6177).
-----------------------------------

describe('npcutil popFromQM pure plan', function()
    local DEFAULT_HIDE = 300

    local function normalizeParams(claim, hide)
        if claim == nil then
            claim = true
        end
        if hide == nil then
            hide = DEFAULT_HIDE
        end
        return { claim = claim, hide = hide }
    end

    local function normalizeMobIds(mobId)
        local mobIds = {}
        if type(mobId) == 'number' then
            mobIds[#mobIds + 1] = mobId
        elseif type(mobId) == 'table' then
            for _, v in pairs(mobId) do
                if type(v) == 'number' then
                    mobIds[#mobIds + 1] = v
                end
            end
        end
        return mobIds
    end

    -- any spawned → cannot pop
    local function canPop(spawnedFlags)
        for _, spawned in ipairs(spawnedFlags) do
            if spawned then
                return false
            end
        end
        return #spawnedFlags > 0
    end

    -- radius offset: r = radius * sqrt(u), theta = v * 2 * pi
    local function radiusOffset(radius, u, v)
        local r = radius * math.sqrt(u)
        local theta = v * 2 * math.pi
        return r * math.cos(theta), r * math.sin(theta)
    end

    local function allDead(aliveFlags)
        for _, alive in ipairs(aliveFlags) do
            if alive then
                return false
            end
        end
        return true
    end

    local function shouldHideQM(hide)
        return hide > 0
    end

    it('defaults claim and hide', function()
        local p = normalizeParams(nil, nil)
        assert(p.claim == true and p.hide == 300)
        p = normalizeParams(false, 0)
        assert(p.claim == false and p.hide == 0)
    end)

    it('normalizes mob ids', function()
        local ids = normalizeMobIds(123)
        assert(#ids == 1 and ids[1] == 123)
        ids = normalizeMobIds({ 1, 2, 'x', 3 })
        assert(#ids == 3 and ids[3] == 3)
        ids = normalizeMobIds('bad')
        assert(#ids == 0)
    end)

    it('can pop only when none spawned and list non-empty', function()
        assert(canPop({ false, false }))
        assert(not canPop({ false, true }))
        assert(not canPop({}))
    end)

    it('radius offset at unit disk center and edge', function()
        local x, z = radiusOffset(10, 0, 0)
        assert(math.abs(x) < 1e-9 and math.abs(z) < 1e-9)
        x, z = radiusOffset(10, 1, 0)
        assert(math.abs(x - 10) < 1e-9 and math.abs(z) < 1e-9)
        x, z = radiusOffset(10, 1, 0.5)
        assert(math.abs(x + 10) < 1e-6 and math.abs(z) < 1e-6)
    end)

    it('all dead check for QM reappear', function()
        assert(allDead({ false, false }))
        assert(not allDead({ false, true }))
        assert(allDead({}))
    end)

    it('hides QM when hide > 0', function()
        assert(shouldHideQM(300) and not shouldHideQM(0))
    end)
end)
