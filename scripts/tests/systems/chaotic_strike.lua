require('scripts/actions/mobskills/chaotic_strike')
describe('Chaotic Strike mob skill', function()
    it('uses three-hit plan and Stun when processed', function()
        local skill = require('scripts/actions/mobskills/chaotic_strike')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=90, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.BLUNT } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 3 and params.fTP[1] == 1.1 and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(damage == 90 and status[1] == xi.effect.STUN and status[4] == 10)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
    end)
end)
