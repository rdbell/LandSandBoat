require('scripts/actions/mobskills/scatter_shell')
describe('Scatter Shell mob skill', function()
    it('splits 1000 damage across targets with skipPDIF', function()
        local shell = require('scripts/actions/mobskills/scatter_shell')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local skill = { getTotalTargets = function() return 4 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 250, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(shell.onMobSkillCheck(target, {}, skill) == 0 and shell.onMobWeaponSkill({}, target, skill, {}) == 250)
        assert(params.baseDamage == 250 and params.skipPDIF and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        shell.onMobWeaponSkill({}, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 250)
    end)
end)
