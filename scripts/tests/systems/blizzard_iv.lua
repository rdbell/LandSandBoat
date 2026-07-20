require('scripts/actions/mobskills/blizzard_iv')
describe('Blizzard IV mob skill', function()
    it('uses Ice magical plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/blizzard_iv')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.element == xi.element.ICE and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 100)
    end)
end)
