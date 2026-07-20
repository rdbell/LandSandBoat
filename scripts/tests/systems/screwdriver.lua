require('scripts/actions/mobskills/screwdriver')
describe('Screwdriver mob skill', function()
    it('uses slashing physical plan with crit and damages only after processing', function()
        local driver = require('scripts/actions/mobskills/screwdriver')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 45 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(driver.onMobSkillCheck(target, mob, {}) == 0 and driver.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.fTP[1] == 1.5 and params.canCrit and params.criticalChance[2] == 0.20 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        driver.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 70)
    end)
end)
