require('scripts/actions/mobskills/warp_out_ealdnarche')
describe('Warp Out Ealdnarche mob skill', function()
    it('teleports near battle target when present', function()
        local skill = require('scripts/actions/mobskills/warp_out_ealdnarche')
        local msg, teleported = nil, false
        local origNear = NearLocation
        NearLocation = function(t, dist, angle) return { x = 1, y = 2, z = 3 } end
        local target = {
            getPos = function() return { x = 0, y = 0, z = 0 } end,
            getRotPos = function() return 0 end,
        }
        local mob = {
            getTarget = function() return target end,
            teleport = function() teleported = true end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(teleported and msg == xi.msg.basic.NONE)

        mob.getTarget = function() return nil end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == nil)
        NearLocation = origNear
    end)
end)
