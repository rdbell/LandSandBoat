require('scripts/actions/mobskills/mighty_snort')
describe('Mighty Snort mob skill', function()
    it('uses Wind magical plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/mighty_snort')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.WIND } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 4.0 and params.dStatMultiplier == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
