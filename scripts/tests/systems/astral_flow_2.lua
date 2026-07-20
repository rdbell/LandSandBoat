require('scripts/actions/mobskills/astral_flow_2')
describe('Astral Flow 2 mob skill', function()
    it('sets USES and pet astralFlowUsed when pet exists', function()
        local skill = require('scripts/actions/mobskills/astral_flow_2')
        local msg, petVar = nil, nil
        local pet = { setLocalVar = function(_, k, v) petVar = { k, v } end }
        local mob = { getPet = function() return nil end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        skill.onMobWeaponSkill(mob, {}, sk, {})
        assert(msg == xi.msg.basic.USES and petVar == nil)
        mob.getPet = function() return pet end
        msg, petVar = nil, nil
        skill.onMobWeaponSkill(mob, {}, sk, {})
        assert(msg == xi.msg.basic.USES and petVar[1] == 'astralFlowUsed' and petVar[2] == 1)
    end)
end)
