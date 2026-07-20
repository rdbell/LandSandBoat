require('scripts/actions/mobskills/chaos_breath')
describe('Chaos Breath mob skill', function()
    it('uses Dark breath plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/chaos_breath')
        local breathMove, processDamage = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v) damage = v end }
        xi.mobskills.mobBreathMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.BREATH, damageType=xi.damageType.DARK } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.percentMultipier == 0.125 and params.damageCap == 700 and params.bonusDamage == 78)
        assert(params.element == xi.element.DARK and params.resistStat == xi.mod.INT)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = breathMove, processDamage
    end)
end)
