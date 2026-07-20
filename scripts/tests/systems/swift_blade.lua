require('scripts/actions/mobskills/swift_blade')
describe('Swift Blade mob skill', function()
    it('readies WS and uses three-hit accuracy-scaled physical plan', function()
        local skill = require('scripts/actions/mobskills/swift_blade')
        local params, basic = nil, nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 90, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = {
            messageBasic = function(_, m, _, p) basic = { m, p } end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(basic[1] == xi.msg.basic.READIES_WS and basic[2] == 41)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 3 and params.accuracyModifier[3] == 60)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
