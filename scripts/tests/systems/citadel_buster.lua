require('scripts/actions/mobskills/citadel_buster')
describe('Citadel Buster mob skill', function()
    it('uses fixed Light plan and resets enmity when processed', function()
        local skill = require('scripts/actions/mobskills/citadel_buster')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, reset = nil, nil, false
        local mob = { resetEnmity = function(_, t) reset = true end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 500, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 500)
        assert(params.baseDamage == 2088 and params.element == xi.element.LIGHT and not reset)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 500)
        assert(damage == 500 and reset)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
