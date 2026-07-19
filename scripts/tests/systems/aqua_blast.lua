require('scripts/actions/mobskills/aqua_blast')

describe('Aqua Blast mob skill', function()
    it('rejects targets behind and uses Water magical parameters before processed damage', function()
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { isBehind = function() return true end, takeDamage = function(_, ...) damage = { ... } end }
        local blast = require('scripts/actions/mobskills/aqua_blast')
        assert(blast.onMobSkillCheck(target, mob, {}) == 1)
        target.isBehind = function() return false end
        xi.mobskills.mobMagicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER } end
        xi.mobskills.processDamage = function() return false end
        assert(blast.onMobSkillCheck(target, mob, {}) == 0 and blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.element == xi.element.WATER and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
    end)
end)
