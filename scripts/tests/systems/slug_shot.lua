require('scripts/actions/mobskills/slug_shot')
describe('Slug Shot mob skill', function()
    it('uses ranged piercing plan with accuracy modifiers', function()
        local shot = require('scripts/actions/mobskills/slug_shot')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(shot.onMobSkillCheck(target, mob, {}) == 0 and shot.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 5 and params.accuracyModifier[1] == -50 and params.skipParry and damage == nil)
        xi.mobskills.processDamage = function() return true end
        shot.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
