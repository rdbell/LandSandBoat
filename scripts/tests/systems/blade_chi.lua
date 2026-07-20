require('scripts/actions/mobskills/blade_chi')
describe('Blade Chi mob skill', function()
    it('uses hybrid Earth plan and sums dual damage when processed', function()
        local skill = require('scripts/actions/mobskills/blade_chi')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damages = nil, {}
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            getHP = function() return 100 end,
            takeDamage = function(_, v, s, a, d)
                table.insert(damages, { v, a, d })
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return {
                damage = 30, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING,
                hybridDamage = 40, hybridAttackType = xi.attackType.MAGICAL, hybridDamageType = xi.damageType.EARTH,
            }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 0)
        assert(params.hybridSkill == true and params.hybridSkillElement == xi.element.EARTH)
        assert(params.numHits == 2 and params.fTP[1] == 0.5 and #damages == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 70)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(#damages == 2 and damages[1][1] == 30 and damages[2][1] == 40)
        assert(damages[2][2] == xi.attackType.MAGICAL and damages[2][3] == xi.damageType.EARTH)
    end)
end)
