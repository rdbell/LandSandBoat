require('scripts/actions/mobskills/mephitic_spore')
describe('Mephitic Spore mob skill', function()
    it('uses Dark breath plan with HIT_DMG and Poison when processed', function()
        local skill = require('scripts/actions/mobskills/mephitic_spore')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.BREATH, damageType=xi.damageType.DARK } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.primaryMessage == xi.msg.basic.HIT_DMG and params.fTP[1] == 4 and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(damage == 80 and status[1] == xi.effect.POISON and status[2] == 50 and status[3] == 3)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
    end)
end)
