require('scripts/actions/mobskills/perdition')
describe('Perdition mob skill', function()
    it('KOs living targets or reports no effect when immune', function()
        local skill = require('scripts/actions/mobskills/perdition')
        local msg, hp = nil, 100
        local target = {
            isUndead = function() return false end,
            hasStatusEffect = function() return false end,
            getMod = function() return 0 end,
            setHP = function(_, v) hp = v end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        -- force death roll to fail by stubbing math.random high and meva 0
        local orig = math.random
        math.random = function() return 100 end
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0)
        assert(msg == xi.msg.basic.FALL_TO_GROUND and hp == 0)
        target.isUndead = function() return true end
        msg, hp = nil, 100
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT and hp == 100)
        math.random = orig
    end)
end)
