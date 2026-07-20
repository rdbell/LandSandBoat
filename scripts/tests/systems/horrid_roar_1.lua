require('scripts/actions/mobskills/horrid_roar_1')
describe('Horrid Roar 1 mob skill', function()
    it('admits front targets and dispels one effect with enmity lower', function()
        local skill = require('scripts/actions/mobskills/horrid_roar_1')
        local msg, lower, front, anim = nil, nil, false, 0
        local mob = {
            getAnimationSub = function() return anim end,
            lowerEnmity = function(_, _, pct) lower = pct end,
        }
        local target = {
            isInfront = function() return front end,
            dispelStatusEffect = function(_, flags)
                assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
                return xi.effect.HASTE
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        front = true
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.HASTE)
        assert(msg == xi.msg.basic.SKILL_ERASE and lower == 20)
        target.dispelStatusEffect = function() return xi.effect.NONE end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.NONE)
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
