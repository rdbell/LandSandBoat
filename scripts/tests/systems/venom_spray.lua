require('scripts/actions/mobskills/venom_spray')
describe('Venom Spray mob skill', function()
    it('uses higher poison power for NM', function()
        local skill = require('scripts/actions/mobskills/venom_spray')
        local status = xi.mobskills.mobStatusEffectMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { isNM = function() return false end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return 456 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.POISON and message == 456)
        assert(params[4] == 15 and params[5] == 3 and params[6] == 120)
        mob.isNM = function() return true end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobStatusEffectMove = status
        assert(params[4] == 25)
    end)
end)
