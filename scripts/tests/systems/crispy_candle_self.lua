require('scripts/actions/mobskills/crispy_candle_self')
describe('Crispy Candle Self mob skill', function()
    it('uses Fire plan with resistTierOverride 0.25', function()
        local skill = require('scripts/actions/mobskills/crispy_candle_self')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.resistTierOverride == 0.25 and params.fTP[1] == 3.5)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
