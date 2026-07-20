require('scripts/actions/mobskills/mijin_gakure')
describe('Mijin Gakure mob skill', function()
    it('uses HP-scaled unaspected magical plan', function()
        local skill = require('scripts/actions/mobskills/mijin_gakure')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local sk = { getMobHPP = function() return 100 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=200, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.ELEMENTAL } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 200)
        assert(params.baseDamage == 506 and params.element == xi.element.NONE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 200)
        assert(damage == 200)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
