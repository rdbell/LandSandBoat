require('scripts/actions/mobskills/stampede')
describe('Stampede mob skill', function()
    it('uses twofold slashing plan and damages only after processing', function()
        local stamp = require('scripts/actions/mobskills/stampede')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(stamp.onMobSkillCheck(target, mob, {}) == 0 and stamp.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.numHits == 2 and params.fTP[1] == 1.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        stamp.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 80)
    end)
end)
