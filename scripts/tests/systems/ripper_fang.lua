require('scripts/actions/mobskills/ripper_fang')
describe('Ripper Fang mob skill', function()
    it('uses blunt physical plan with attack multiplier and damages only after processing', function()
        local fang = require('scripts/actions/mobskills/ripper_fang')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(fang.onMobSkillCheck(target, mob, {}) == 0 and fang.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 1.5 and params.attackMultiplier[1] == 1.3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        fang.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
