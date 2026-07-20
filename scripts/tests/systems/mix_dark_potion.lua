require('scripts/actions/mobskills/mix_dark_potion')
describe('Mix Dark Potion mob skill', function()
    it('uses fixed 666 damage magical plan', function()
        local skill = require('scripts/actions/mobskills/mix_dark_potion')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=666, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.NONE } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 666)
        assert(params.baseDamage == 666 and params.skipDamageAdjustment and params.skipMagicBonusDiff)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 666)
        assert(damage == 666)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
