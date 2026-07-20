require('scripts/actions/mobskills/snowball')
describe('Snowball mob skill', function()
    it('uses ice magical plan and damages only after processing', function()
        local ball = require('scripts/actions/mobskills/snowball')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(ball.onMobSkillCheck(target, mob, {}) == 0 and ball.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.5 and params.element == xi.element.ICE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        ball.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 70)
    end)
end)
