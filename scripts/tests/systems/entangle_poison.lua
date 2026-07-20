require('scripts/actions/mobskills/entangle_poison')
describe('Entangle Poison mob skill', function()
    it('uses physical plan with Bind and Poison and breakBind false', function()
        local skill = require('scripts/actions/mobskills/entangle_poison')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, opts, statuses = nil, nil, nil, {}
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            takeDamage = function(_, v, _, _, _, o) damage = v; opts = o end,
        }
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.SLASHING } end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) statuses[#statuses+1]={e,p,t,d} end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.0 and #statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100 and opts.breakBind == false)
        assert(statuses[1][1] == xi.effect.BIND and statuses[2][1] == xi.effect.POISON and statuses[2][2] == 50)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
