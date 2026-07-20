require('scripts/actions/mobskills/arcuballista')
describe('Arcuballista mob skill', function()
    it('uses ranged plan with accuracyModifier 100 and TP-scaled fTP', function()
        local skill = require('scripts/actions/mobskills/arcuballista')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 35 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 95, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 95)
        assert(params.fTP[1] == 2.5 and params.fTP[2] == 3.0 and params.fTP[3] == 4.0)
        assert(params.accuracyModifier[1] == 100 and params.skipParry and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 95)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 95)
    end)
end)
