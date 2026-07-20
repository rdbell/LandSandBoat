require('scripts/actions/mobskills/split_shot')
describe('Split Shot mob skill', function()
    it('uses ranged piercing plan with ignoreDefense and damages only after processing', function()
        local shot = require('scripts/actions/mobskills/split_shot')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(shot.onMobSkillCheck(target, mob, {}) == 0 and shot.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.ignoreDefense[2] == 0.35 and params.ignoreDefense[3] == 0.5 and params.accuracyModifier[1] == 30 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        shot.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
