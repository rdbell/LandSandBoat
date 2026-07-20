require('scripts/actions/mobskills/vampiric_lash')
describe('Vampiric Lash mob skill', function()
    it('uses skip-PDIF piercing plan and HP drain after processing', function()
        local skill = require('scripts/actions/mobskills/vampiric_lash')
        local move, process, drain = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getWeaponDmg = function() return 80 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobDrainMove = function() return 123 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(params.skipPDIF and params.damageType == xi.damageType.PIERCING and message == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123)
    end)
end)
