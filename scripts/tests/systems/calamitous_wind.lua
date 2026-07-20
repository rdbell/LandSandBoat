require('scripts/actions/mobskills/calamitous_wind')
describe('Calamitous Wind mob skill', function()
    it('admits under 50 HPP and uses Wind plan with full dispel', function()
        local skill = require('scripts/actions/mobskills/calamitous_wind')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, dispelFlag = nil, nil, nil
        local mob = { getHPP = function() return 50 end, getMainLvl = function() return 60 end }
        local target = {
            takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end,
            dispelAllStatusEffect = function(_, flag) dispelFlag = flag end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getHPP = function() return 49 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.element == xi.element.WIND and params.fTP[1] == 4 and dispelFlag == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(damage[1] == 120)
        assert(dispelFlag == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
