require('scripts/actions/mobskills/apex_arrow')
describe('Apex Arrow mob skill', function()
    it('uses ranged plan with ignoreDefense and skip flags', function()
        local skill = require('scripts/actions/mobskills/apex_arrow')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.fTP[1] == 3.0 and params.ignoreDefense[1] == 0.15 and params.ignoreDefense[2] == 0.35 and params.ignoreDefense[3] == 0.5)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.attackType == xi.attackType.RANGED and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 120)
    end)
end)
