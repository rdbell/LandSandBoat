require('scripts/actions/mobskills/reaving_wind_kb')
describe('Reaving Wind KB mob skill', function()
    it('sets NONE and negates knockback when move speed stacked low', function()
        local skill = require('scripts/actions/mobskills/reaving_wind_kb')
        local msg, kb = nil, nil
        local target = {
            getMod = function() return -11 end,
            getID = function() return 5 end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        local action = { knockback = function(_, id, k) kb = { id, k } end }
        skill.onMobWeaponSkill({}, target, sk, action)
        assert(msg == xi.msg.basic.NONE and kb[1] == 5 and kb[2] == xi.action.knockback.NONE)
        target.getMod = function() return 0 end
        kb = nil
        skill.onMobWeaponSkill({}, target, sk, action)
        assert(kb == nil)
    end)
end)
