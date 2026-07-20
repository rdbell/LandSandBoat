require('scripts/actions/mobskills/divine_spear')
describe('Divine Spear mob skill', function()
    it('rejects behind targets and uses Light plan', function()
        local skill = require('scripts/actions/mobskills/divine_spear')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local behind = true
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            isBehind = function() return behind end,
            takeDamage = function(_, v) damage = v end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        behind = false
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=200, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 12.5 and damage == 200)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
