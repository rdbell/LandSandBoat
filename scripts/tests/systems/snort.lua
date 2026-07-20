require('scripts/actions/mobskills/snort')
describe('Snort mob skill', function()
    it('uses wind magical plan and damages only after processing', function()
        local snort = require('scripts/actions/mobskills/snort')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(snort.onMobSkillCheck(target, mob, {}) == 0 and snort.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.element == xi.element.WIND and damage == nil)
        xi.mobskills.processDamage = function() return true end
        snort.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
