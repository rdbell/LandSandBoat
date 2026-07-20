require('scripts/actions/mobskills/mine_blast')

describe('Mine Blast mob skill', function()
    it('uses its Fire magical plan, damages only after processing, then self-destructs with animation', function()
        local mine = require('scripts/actions/mobskills/mine_blast')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, anim, finalized = nil, nil, nil, nil
        local mob = {
            getMainLvl = function() return 75 end,
            entityAnimationPacket = function(_, value) anim = value end,
            setHP = function(_, value) finalized = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(mine.onMobSkillCheck(target, mob, skill) == 0 and mine.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 100 and params.fTP[2] == 100 and params.fTP[3] == 100)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        mine.onMobWeaponSkill(mob, target, skill, {})
        mine.onMobSkillFinalize(mob, skill)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
        assert(anim == 'mai1' and finalized == 0)
    end)
end)
