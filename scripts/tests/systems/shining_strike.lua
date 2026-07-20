require('scripts/actions/mobskills/shining_strike')
describe('Shining Strike mob skill', function()
    it('uses light magical plan and damages only after processing', function()
        local strike = require('scripts/actions/mobskills/shining_strike')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 88, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(strike.onMobSkillCheck(target, mob, {}) == 0 and strike.onMobWeaponSkill(mob, target, {}, {}) == 88)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.0 and params.fTP[2] == 1.75 and params.fTP[3] == 2.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        strike.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 88)
    end)
end)
