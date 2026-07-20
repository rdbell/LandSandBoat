require('scripts/actions/mobskills/warp_in_ealdnarche')
describe('Warp In Ealdnarche mob skill', function()
    it('uses TELEPORT_END ability and sets NONE', function()
        local skill = require('scripts/actions/mobskills/warp_in_ealdnarche')
        local msg, used = nil, nil
        local mob = {
            getMobMod = function() return 123 end,
            useMobAbility = function(_, id) used = id end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(used == 123 and msg == xi.msg.basic.NONE)
    end)
end)
