require('scripts/actions/mobskills/blast_arrow')
describe('Blast Arrow mob skill', function()
    it('uses ranged plan with accuracyModifier and skip flags', function()
        local skill = require('scripts/actions/mobskills/blast_arrow')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2.0 and params.accuracyModifier[3] == 60 and params.skipParry and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 80)
    end)
end)
