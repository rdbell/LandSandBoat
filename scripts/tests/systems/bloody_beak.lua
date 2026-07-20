require('scripts/actions/mobskills/bloody_beak')
describe('Bloody Beak mob skill', function()
    it('uses physical plan and HP drain message when processed', function()
        local skill = require('scripts/actions/mobskills/bloody_beak')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local drainMove = xi.mobskills.mobDrainMove
        local params, message = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local sk = { setMsg = function(_, v) message = v end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function() return 55 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        assert(params.numHits == 3 and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobDrainMove = drainMove
        assert(message == 55)
    end)
end)
