require('scripts/actions/mobskills/bai_wing')
describe('Bai Wing mob skill', function()
    it('admits when flying and uses Earth plan with Slow 9000', function()
        local skill = require('scripts/actions/mobskills/bai_wing')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = {
            getAnimationSub = function() return 0 end,
            getMainLvl = function() return 50 end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 140, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 140)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.element == xi.element.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 140)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 140 and effect[1] == xi.effect.SLOW and effect[2] == 9000 and effect[4] == 120)
    end)
end)
