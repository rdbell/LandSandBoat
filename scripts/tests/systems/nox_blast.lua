require('scripts/actions/mobskills/nox_blast')

describe('Nox Blast mob skill', function()
    it('requires standing animation and resets TP only after processing', function()
        local blast = require('scripts/actions/mobskills/nox_blast')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, tp = nil, nil, 500
        local animSub = 1
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainLvl = function() return 75 end,
        }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            setTP = function(_, value) tp = value end,
        }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end

        animSub = 0
        assert(blast.onMobSkillCheck(target, mob, skill) == 1)
        animSub = 1
        assert(blast.onMobSkillCheck(target, mob, skill) == 0)
        assert(blast.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and tp == 500)

        xi.mobskills.processDamage = function() return true end
        blast.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(tp == 0)
    end)
end)
