require('scripts/actions/mobskills/hot_shot')
describe('Hot Shot mob skill', function()
    it('uses hybrid ranged Fire plan and sums damages', function()
        local skill = require('scripts/actions/mobskills/hot_shot')
        local rangedMove, processDamage = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        local params, damages = nil, {}
        local hp = 100
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            getHP = function() return hp end,
            takeDamage = function(_, v, _, atk, dmg)
                damages[#damages+1] = { v, atk, dmg }
            end,
        }
        xi.mobskills.mobRangedMove = function(_,_,_,_,v)
            params = v
            return {
                damage = 40, hybridDamage = 30,
                attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING,
                hybridAttackType = xi.attackType.MAGICAL, hybridDamageType = xi.damageType.FIRE,
            }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 0)
        assert(params.hybridSkill and params.hybridSkillElement == xi.element.FIRE)
        assert(params.fTP[1] == 0.5 and params.fTP[3] == 1.0 and #damages == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(#damages == 2 and damages[1][1] == 40 and damages[2][1] == 30)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = rangedMove, processDamage
    end)
end)
