require('scripts/actions/mobskills/choke_breath')
describe('Choke Breath mob skill', function()
    it('uses fixed base physical plan with Paralysis and Silence', function()
        local skill = require('scripts/actions/mobskills/choke_breath')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=80, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) statuses[#statuses+1]={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 80)
        assert(params.baseDamage == 100 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(#statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill({}, target, {}, {}) == 80)
        assert(damage == 80 and statuses[1][1] == xi.effect.PARALYSIS and statuses[2][1] == xi.effect.SILENCE)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
    end)
end)
