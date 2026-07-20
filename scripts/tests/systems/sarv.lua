require('scripts/actions/mobskills/sarv')
describe('Sarv mob skill', function()
    it('uses ranged piercing plan and damages only after processing', function()
        local sarv = require('scripts/actions/mobskills/sarv')
        local move, process = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(sarv.onMobSkillCheck(target, mob, {}) == 0 and sarv.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.75 and params.fTP[2] == 5.5 and params.fTP[3] == 8.25)
        assert(params.skipParry and params.skipGuard and params.skipBlock and damage == nil)
        xi.mobskills.processDamage = function() return true end
        sarv.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
