require('scripts/actions/mobskills/vorpal_blade')
describe('Vorpal Blade mob skill', function()
    it('uses four-hit crit physical plan', function()
        local skill = require('scripts/actions/mobskills/vorpal_blade')
        local params, basic = nil, nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = {
            getFamily = function() return 0 end,
            getPool = function() return 0 end,
            messageBasic = function(_, m, _, p) basic = { m, p } end,
            getWeaponDmg = function() return 40 end,
        }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(basic[1] == xi.msg.basic.READIES_WS and basic[2] == 40)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 4 and params.canCrit and params.criticalChance[3] == 0.5)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
