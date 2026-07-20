require('scripts/actions/mobskills/teraflare')
describe('Teraflare mob skill', function()
    it('uses fire magical fTP 19 and penalizes secondary targets', function()
        local skill = require('scripts/actions/mobskills/teraflare')
        local params, dmg = nil, nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 500, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function(mob, target, sk, action, info)
            dmg = info.damage
            return true
        end
        local mob = { getMainLvl = function() return 80 end }
        local target = { getID = function() return 2 end, takeDamage = function() end }
        local sk = { getPrimaryTargetID = function() return 1 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 200)
        assert(params.fTP[1] == 19 and params.dStatMultiplier == 1.5 and dmg == 200)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
