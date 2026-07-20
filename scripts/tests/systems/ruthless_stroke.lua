require('scripts/actions/mobskills/ruthless_stroke')
describe('Ruthless Stroke mob skill', function()
    it('uses fourfold piercing plan with TP-scaled fTP and damages only after processing', function()
        local stroke = require('scripts/actions/mobskills/ruthless_stroke')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(stroke.onMobSkillCheck(target, mob, {}) == 0 and stroke.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.fTP[1] == 5.375 and params.fTP[2] == 14 and params.fTP[3] == 23 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        stroke.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
