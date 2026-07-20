require('scripts/actions/mobskills/scouring_bubbles')
describe('Scouring Bubbles mob skill', function()
    it('uses water magical plan and damages only after processing', function()
        local bubbles = require('scripts/actions/mobskills/scouring_bubbles')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 220, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(bubbles.onMobSkillCheck(target, mob, {}) == 0 and bubbles.onMobWeaponSkill(mob, target, {}, {}) == 220)
        assert(params.baseDamage == 40 and params.fTP[1] == 12.25 and params.element == xi.element.WATER and damage == nil)
        xi.mobskills.processDamage = function() return true end
        bubbles.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 220)
    end)
end)
