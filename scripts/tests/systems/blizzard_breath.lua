require('scripts/actions/mobskills/blizzard_breath')
describe('Blizzard Breath mob skill', function()
    it('uses Ice breath plan with level-scaled bonus', function()
        local skill = require('scripts/actions/mobskills/blizzard_breath')
        local breathMove = xi.mobskills.mobBreathMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.percentMultipier == 0.125 and params.damageCap == 700 and params.bonusDamage == 78)
        assert(params.element == xi.element.ICE and params.resistStat == xi.mod.INT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        xi.mobskills.mobBreathMove = breathMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 200)
    end)
end)
