require('scripts/actions/mobskills/sidewinder')
describe('Sidewinder mob skill', function()
    it('uses ranged piercing plan with accuracy modifiers and damages only after processing', function()
        local wind = require('scripts/actions/mobskills/sidewinder')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(wind.onMobSkillCheck(target, mob, {}) == 0 and wind.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 5 and params.accuracyModifier[1] == -50 and params.skipParry and damage == nil)
        xi.mobskills.processDamage = function() return true end
        wind.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 200)
    end)
end)
