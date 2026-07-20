require('scripts/actions/mobskills/sunburst')
describe('Sunburst mob skill', function()
    it('uses TP-scaled magical plan with light or dark element', function()
        local skill = require('scripts/actions/mobskills/sunburst')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local random = math.random
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        math.random = function(a, b) assert(a == 1 and b == 100); return 25 end -- light
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = value.damageType }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.0 and params.fTP[2] == 2.5 and params.fTP[3] == 4.0)
        assert(params.element == xi.element.LIGHT and params.dStatMultiplier == 1 and damage == nil)
        math.random = function() return 75 end -- dark
        skill.onMobWeaponSkill(mob, target, {}, {})
        assert(params.element == xi.element.DARK)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        math.random = random
        assert(damage[1] == 80)
    end)
end)
