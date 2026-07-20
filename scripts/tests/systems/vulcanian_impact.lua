require('scripts/actions/mobskills/vulcanian_impact')
describe('Vulcanian Impact mob skill', function()
    it('uses fTP 2 by default and fTP 4 for IMPACT_2 skill id', function()
        local skill = require('scripts/actions/mobskills/vulcanian_impact')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local sk = { getID = function() return 0 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 2.0 and params.element == xi.element.FIRE and damage == nil)
        sk.getID = function() return xi.mobSkill.VULCANIAN_IMPACT_2 end
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(params.fTP[1] == 4.0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
