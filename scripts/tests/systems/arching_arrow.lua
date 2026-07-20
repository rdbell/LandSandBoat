require('scripts/actions/mobskills/arching_arrow')
describe('Arching Arrow mob skill', function()
    it('uses ranged plan with TP-scaled critical chance', function()
        local skill = require('scripts/actions/mobskills/arching_arrow')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 45 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 110, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 110)
        assert(params.fTP[1] == 3.5 and params.criticalChance[1] == 0.1 and params.criticalChance[2] == 0.3 and params.criticalChance[3] == 0.5)
        assert(params.skipParry and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 110)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 110)
    end)
end)
