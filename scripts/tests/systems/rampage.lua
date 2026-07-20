require('scripts/actions/mobskills/rampage')
describe('Rampage mob skill', function()
    it('uses its fivefold slashing plan with crit chances and damages only after processing', function()
        local rampage = require('scripts/actions/mobskills/rampage')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(rampage.onMobSkillCheck(target, mob, {}) == 0 and rampage.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 5 and params.fTP[1] == 0.5 and params.canCrit and params.criticalChance[3] == 0.50 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rampage.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
