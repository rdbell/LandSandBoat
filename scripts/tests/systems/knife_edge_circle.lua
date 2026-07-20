describe('Knife Edge Circle mob skill', function()
    it('requests Stun then Poison, returns the first successful effect, and misses when both fail', function()
        local circle = require('scripts/actions/mobskills/knife_edge_circle')
        local status = xi.mobskills.mobStatusEffectMove
        local calls, message = {}, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        local results = { xi.msg.basic.SKILL_ENFEEB_IS, xi.msg.basic.SKILL_ENFEEB_IS }
        xi.mobskills.mobStatusEffectMove = function(...) calls[#calls + 1] = { ... }; return results[#calls] end
        assert(circle.onMobSkillCheck(target, mob, skill) == 0 and circle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.STUN)
        assert(calls[1][3] == xi.effect.STUN and calls[1][4] == 1 and calls[1][5] == 0 and calls[1][6] == 15 and calls[2][3] == xi.effect.POISON and calls[2][4] == 20 and calls[2][5] == 0 and calls[2][6] == 120 and message == xi.msg.basic.SKILL_ENFEEB_IS)
        calls, results = {}, { 0, xi.msg.basic.SKILL_ENFEEB_IS }; assert(circle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.POISON and message == xi.msg.basic.SKILL_ENFEEB_IS)
        calls, results = {}, { 0, 0 }; assert(circle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.NONE and message == xi.msg.basic.SKILL_MISS)
        xi.mobskills.mobStatusEffectMove = status
    end)
end)
