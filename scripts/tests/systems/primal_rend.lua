require('scripts/actions/mobskills/primal_rend')

describe('Primal Rend mob skill', function()
    it('uses its light magical plan with dStat and damages only after processing', function()
        local rend = require('scripts/actions/mobskills/primal_rend')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(rend.onMobSkillCheck(target, mob, {}) == 0 and rend.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.fTP[2] == 4.25 and params.fTP[3] == 4.75)
        assert(params.element == xi.element.LIGHT and params.dStatMultiplier == 1.5)
        assert(params.dStatAttackerMod == xi.mod.CHR and params.dStatDefenderMod == xi.mod.INT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rend.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[4] == xi.damageType.LIGHT)
    end)
end)
