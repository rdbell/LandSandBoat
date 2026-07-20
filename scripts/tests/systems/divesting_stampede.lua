require('scripts/actions/mobskills/divesting_stampede')
describe('Divesting Stampede mob skill', function()
    it('uses physical plan and dual defense downs when processed', function()
        local skill = require('scripts/actions/mobskills/divesting_stampede')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.PIERCING } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) statuses[#statuses+1]={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.0 and #statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100 and statuses[1][1] == xi.effect.MAGIC_DEF_DOWN and statuses[2][1] == xi.effect.DEFENSE_DOWN)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
