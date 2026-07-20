require('scripts/actions/mobskills/heavy_shot')
describe('Heavy Shot mob skill', function()
    it('uses ranged crit plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/heavy_shot')
        local rangedMove = xi.mobskills.mobRangedMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobRangedMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.RANGED, damageType=xi.damageType.PIERCING } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.5 and params.canCrit and params.criticalChance[3] == 0.5)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobRangedMove = rangedMove
        xi.mobskills.processDamage = processDamage
    end)
end)
