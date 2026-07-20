require('scripts/actions/mobskills/shark_bite')
describe('Shark Bite mob skill', function()
    it('uses twofold piercing plan and damages only after processing', function()
        local bite = require('scripts/actions/mobskills/shark_bite')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(bite.onMobSkillCheck(target, mob, {}) == 0 and bite.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 2 and params.fTP[1] == 2.0 and params.fTP[2] == 2.5 and params.fTP[3] == 3.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        bite.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 90)
    end)
end)
