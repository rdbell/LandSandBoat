require('scripts/actions/mobskills/antimatter')
describe('Antimatter mob skill', function()
    it('uses the level-65 Light magical damage-cap boundary and damages only after processing', function()
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil; local level = 64
        local mob = { getMainLvl = function() return level end }; local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return false end
        local antimatter = require('scripts/actions/mobskills/antimatter')
        assert(antimatter.onMobSkillCheck(target,mob,{}) == 0 and antimatter.onMobWeaponSkill(mob,target,{},{}) == 123 and params.baseDamage == 750 and damage == nil)
        level = 65; xi.mobskills.processDamage = function() return true end; antimatter.onMobWeaponSkill(mob,target,{},{})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(params.baseDamage == 1500 and params.fTP[1] == 1 and params.element == xi.element.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
    end)
end)
