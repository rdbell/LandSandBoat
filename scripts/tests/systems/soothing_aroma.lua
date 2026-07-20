require('scripts/actions/mobskills/soothing_aroma')
describe('Soothing Aroma mob skill', function()
    it('gates Raskovnik HP and charms PC targets on ENFEEB_IS', function()
        local aroma = require('scripts/actions/mobskills/soothing_aroma')
        local status = xi.mobskills.mobStatusEffectMove
        local hpp, pool, isPC, params, message, charmed = 60, xi.mobPool.RASKOVNIK, true, nil, nil, false
        local mob = {
            getHPP = function() return hpp end,
            getPool = function() return pool end,
            charm = function(_, t) charmed = true end,
        }
        local target = { isPC = function() return isPC end }
        local skill = { setMsg = function(_, value) message = value end }
        assert(aroma.onMobSkillCheck(target, mob, skill) == 1)
        hpp = 40; assert(aroma.onMobSkillCheck(target, mob, skill) == 0)
        isPC = false
        assert(aroma.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.CHARM_I)
        assert(message == xi.msg.basic.SKILL_MISS and not charmed)
        isPC = true
        xi.mobskills.mobStatusEffectMove = function(...) params = { ... }; return xi.msg.basic.SKILL_ENFEEB_IS end
        assert(aroma.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.CHARM_I)
        xi.mobskills.mobStatusEffectMove = status
        assert(params[3] == xi.effect.CHARM_I and params[4] == 0 and params[5] == 3 and params[6] == 150)
        assert(charmed and message == xi.msg.basic.SKILL_ENFEEB_IS)
    end)
end)
