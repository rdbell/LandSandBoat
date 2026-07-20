require('scripts/actions/mobskills/ocher_blast')

describe('Ocher Blast mob skill', function()
    it('requires animation sub 1 and uses Earth magical plan after processing', function()
        local blast = require('scripts/actions/mobskills/ocher_blast')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, animSub = nil, nil, 0
        local mob = {
            getAnimationSub = function() return animSub end,
            getMainLvl = function() return 75 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        assert(blast.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 1
        assert(blast.onMobSkillCheck(target, mob, {}) == 0)
        assert(blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3.5 and params.fTP[2] == 3.875 and params.fTP[3] == 4.25)
        assert(params.element == xi.element.EARTH and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.EARTH)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        blast.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.EARTH)
    end)
end)
