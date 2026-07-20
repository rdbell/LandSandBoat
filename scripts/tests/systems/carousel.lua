require('scripts/actions/mobskills/carousel')
describe('Carousel mob skill', function()
    it('uses its physical plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/carousel')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        local mobNM = { isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end, getWeaponDmg = function() return 50 end }
        assert(skill.onMobSkillCheck({}, mobNM, {}) == 0)
        local mob = { isMobType = function() return false end, getWeaponDmg = function() return 50 end }
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob = mobNM
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 3 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage[1] == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
