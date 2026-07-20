require('scripts/actions/mobskills/divine_judgment')
describe('Divine Judgment mob skill', function()
    it('uses Light wipe-shadows plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/divine_judgment')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=200, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 20 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(damage == 200)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
