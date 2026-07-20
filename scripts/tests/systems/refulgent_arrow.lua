require('scripts/actions/mobskills/refulgent_arrow')
describe('Refulgent Arrow mob skill', function()
    it('uses its twofold ranged piercing plan and damages only after processing', function()
        local arrow = require('scripts/actions/mobskills/refulgent_arrow')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(arrow.onMobSkillCheck(target, mob, {}) == 0 and arrow.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 2 and params.fTP[1] == 3 and params.fTP[2] == 4.25 and params.fTP[3] == 5)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil)
        xi.mobskills.processDamage = function() return true end
        arrow.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
