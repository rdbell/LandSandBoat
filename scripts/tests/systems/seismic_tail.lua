require('scripts/actions/mobskills/seismic_tail')
describe('Seismic Tail mob skill', function()
    it('uses blunt physical plan with crit and damages only after processing', function()
        local tail = require('scripts/actions/mobskills/seismic_tail')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(tail.onMobSkillCheck(target, mob, {}) == 0 and tail.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2 and params.canCrit and params.criticalChance[3] == 0.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        tail.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
