require('scripts/actions/mobskills/boreas_mantle')
describe('Boreas Mantle mob skill', function()
    it('plans four clones with HP scaling and NONE message', function()
        local skill = require('scripts/actions/mobskills/boreas_mantle')
        local spawned, msg, timerMs = {}, nil, nil
        local player = {}
        local mob = {
            getID = function() return 100 end,
            getTarget = function() return player end,
            getHPP = function() return 50 end,
            getXPos = function() return 1 end,
            getYPos = function() return 2 end,
            getZPos = function() return 3 end,
            timer = function(_, ms, _) timerMs = ms end,
        }
        stub('SpawnMob', function(id)
            local clone = {
                id = id,
                maxhp = nil,
                hp = nil,
                setMaxHP = function(self, v) self.maxhp = v end,
                setHP = function(self, v) self.hp = v end,
                updateEnmity = function(self, p) self.enmity = p end,
                setPos = function(self, x, y, z) self.pos = { x, y, z } end,
            }
            spawned[#spawned + 1] = clone
            return clone
        end)
        stub('GetMobByID', function() return nil end)
        local skillObj = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, skillObj) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, skillObj, {}) == 0)
        assert(#spawned == 4)
        assert(spawned[1].hp == 500 and spawned[1].maxhp == 1000)
        assert(spawned[1].pos[1] == 1 and spawned[4].id == 104)
        assert(spawned[1].enmity == player)
        assert(timerMs == 30000)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
