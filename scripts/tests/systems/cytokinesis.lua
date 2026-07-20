require('scripts/actions/mobskills/cytokinesis')
describe('Cytokinesis mob skill', function()
    it('spawns zone pet reinforcements at mob position', function()
        local skill = require('scripts/actions/mobskills/cytokinesis')
        local spawned, enmity = {}, false
        local pet = {
            isSpawned = function() return false end,
            setSpawn = function() end,
            setPos = function() end,
            getID = function() return 101 end,
            updateEnmity = function() enmity = true end,
        }
        stub('GetMobByID', function(id)
            if id == 101 then return pet end
            return nil
        end)
        stub('SpawnMob', function(id) spawned[#spawned+1] = id end)
        local mob = {
            getZoneID = function() return 1 end,
            getID = function() return 100 end,
            getXPos = function() return 1 end,
            getYPos = function() return 2 end,
            getZPos = function() return 3 end,
        }
        -- inject zone pet table
        zones[1] = zones[1] or {}
        zones[1].pet = { [100] = { 101 } }
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, {}, {}) == 0)
        assert(spawned[1] == 101 and enmity)
    end)
end)
