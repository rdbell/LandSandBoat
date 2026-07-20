require('scripts/actions/mobskills/blitz')
describe('Blitz mob skill', function()
    it('uses five-hit TP-scaled fTP plan', function()
        local skill = require('scripts/actions/mobskills/blitz')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.numHits == 5 and params.fTP[1] == 1.5 and params.fTP[2] == 7.0 and params.fTP[3] == 12.5)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 120)
    end)
end)
