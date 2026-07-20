describe('Ill Wind mob skill', function()
    it('has no explicit skill-check result and dispels one effect only after processing damage', function()
        local wind = require('scripts/actions/mobskills/ill_wind')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local params, damage, dispels = nil, nil, 0
        local mob = { getMainLvl = function() return 75 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            dispelStatusEffect = function() dispels = dispels + 1 end,
        }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end

        assert(wind.onMobSkillCheck(target, mob, {}) == nil)
        assert(wind.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3.5 and params.fTP[2] == 3.5 and params.fTP[3] == 3.5)
        assert(params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WIND)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and dispels == 0)

        xi.mobskills.processDamage = function() return true end
        wind.onMobWeaponSkill(mob, target, {}, {})

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
        assert(dispels == 1)
    end)
end)
