require('scripts/actions/mobskills/circle_of_flames')
describe('Circle of Flames mob skill', function()
    it('rejects one-bomb left and scales additive damage by animation sub', function()
        local skill = require('scripts/actions/mobskills/circle_of_flames')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local anim = 2
        local mob = {
            getAnimationSub = function() return anim end,
            getMainLvl = function() return 50 end,
        }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        anim = 5
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.FIRE } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.additiveDamage[1] == 25 and params.fTP[1] == 0.5 and params.element == xi.element.FIRE)
        assert(status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(damage == 80 and status[1] == xi.effect.WEIGHT and status[2] == 20 and status[4] == 120)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
    end)
end)
