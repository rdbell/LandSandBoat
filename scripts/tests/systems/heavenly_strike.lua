require('scripts/actions/mobskills/heavenly_strike')
describe('Heavenly Strike mob skill', function()
    it('uses Ice magical plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/heavenly_strike')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.7 and params.element == xi.element.ICE)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
