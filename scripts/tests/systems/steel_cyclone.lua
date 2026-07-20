require('scripts/actions/mobskills/steel_cyclone')
describe('Steel Cyclone mob skill', function()
    it('uses slashing physical plan with attack multiplier', function()
        local cyc = require('scripts/actions/mobskills/steel_cyclone')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(cyc.onMobSkillCheck(target, mob, {}) == 0 and cyc.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.fTP[1] == 1.5 and params.fTP[2] == 1.75 and params.fTP[3] == 3.0 and params.attackMultiplier[1] == 1.66 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        cyc.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 150)
    end)
end)
