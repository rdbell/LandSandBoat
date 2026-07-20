require('scripts/actions/mobskills/sanguine_blade')
describe('Sanguine Blade mob skill', function()
    it('uses dark magical plan with TP drain and skips undead drain', function()
        local blade = require('scripts/actions/mobskills/sanguine_blade')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, hpAdd, undead = nil, nil, nil, false
        local tp = 1000
        local mob = {
            getMainLvl = function() return 50 end,
            addHP = function(_, v) hpAdd = v end,
        }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            isUndead = function() return undead end,
        }
        local skill = { getTP = function() return tp end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        assert(blade.onMobSkillCheck(target, mob, skill) == 0 and blade.onMobWeaponSkill(mob, target, skill, {}) == 200)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.75 and params.element == xi.element.DARK)
        assert(params.dStatMultiplier == 2 and damage == nil and hpAdd == nil)
        xi.mobskills.processDamage = function() return true end
        blade.onMobWeaponSkill(mob, target, skill, {})
        assert(damage[1] == 200 and hpAdd == 100)
        undead, hpAdd = true, nil
        blade.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(hpAdd == nil)
    end)
end)
