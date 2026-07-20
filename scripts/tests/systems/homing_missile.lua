require('scripts/actions/mobskills/homing_missile')
describe('Homing Missile mob skill', function()
    it('uses 90 percent HP base and resets enmity when processed', function()
        local skill = require('scripts/actions/mobskills/homing_missile')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, reset = nil, nil, false
        local mob = { resetEnmity = function() reset = true end }
        local target = {
            getHP = function() return 1000 end,
            takeDamage = function(_, v) damage = v end,
        }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=900, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.ELEMENTAL } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 900)
        assert(params.baseDamage == 900 and not reset)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 900)
        assert(damage == 900 and reset)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
