require('scripts/actions/mobskills/zeid_summon_shadows')
describe('Zeid Summon Shadows mob skill', function()
    it('always denies check and spawns two free shadows with random offsets', function()
        local skill = require('scripts/actions/mobskills/zeid_summon_shadows')
        local origGet = _G.GetMobByID
        local origRandom = math.random
        local message, spawned, positions, enmity = nil, {}, {}, {}
        local rolls = { 1, -1, 2, 0 }
        local ri = 0
        math.random = function(a, b)
            assert(a == -2 and b == 2)
            ri = ri + 1
            return rolls[ri]
        end
        local function makeShadow(id)
            return {
                isSpawned = function() return false end,
                spawn = function(self) spawned[id] = true end,
                setPos = function(self, x, y, z) positions[id] = { x, y, z } end,
                updateEnmity = function(self, t) enmity[id] = t end,
            }
        end
        local shadows = { [101] = makeShadow(101), [102] = makeShadow(102) }
        _G.GetMobByID = function(id) return shadows[id] end
        local mob = {
            getID = function() return 100 end,
            getXPos = function() return 10 end,
            getYPos = function() return 5 end,
            getZPos = function() return 20 end,
        }
        local target = {}
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 0)
        _G.GetMobByID = origGet
        math.random = origRandom
        assert(spawned[101] and spawned[102])
        assert(positions[101][1] == 11 and positions[101][2] == 5 and positions[101][3] == 19)
        assert(positions[102][1] == 12 and positions[102][3] == 20)
        assert(enmity[101] == target and enmity[102] == target)
        assert(message == xi.msg.basic.NONE)
    end)
end)
