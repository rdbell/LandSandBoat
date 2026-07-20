require('scripts/actions/mobskills/hysteric_assault')
describe('Hysteric Assault mob skill', function()
    it('uses five-hit crit plan and HP drain message when processed', function()
        local skill = require('scripts/actions/mobskills/hysteric_assault')
        local physicalMove, processDamage, drainMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, msg = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobPhysicalMove = function(_,_,_,_,v) params=v; return { damage=100, attackType=xi.attackType.PHYSICAL, damageType=xi.damageType.BLUNT } end
        xi.mobskills.mobDrainMove = function() return 55 end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(params.numHits == 5 and params.canCrit and params.criticalChance[3] == 0.25)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5 and msg == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(msg == 55)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = physicalMove, processDamage, drainMove
    end)
end)
