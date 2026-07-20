require('scripts/actions/mobskills/lamentation')

describe('Lamentation mob skill', function()
    it('uses Light magical parameters and applies non-erasable Dia only after processing succeeds', function()
        local lamentation = require('scripts/actions/mobskills/lamentation')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied, removed = nil, nil, nil, nil
        local effect = { delEffectFlag = function(_, flag) removed = flag end }
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            getStatusEffect = function(_, id) assert(id == xi.effect.DIA); return effect end,
        }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end

        assert(lamentation.onMobSkillCheck(target, mob, {}) == 0)
        assert(lamentation.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and params.element == xi.element.LIGHT)
        assert(params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and applied == nil and removed == nil)

        xi.mobskills.processDamage = function() return true end
        assert(lamentation.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
        assert(applied[1] == mob and applied[2] == target and applied[3] == xi.effect.DIA)
        assert(applied[4] == 8 and applied[5] == 3 and applied[6] == 30 and applied[7] == 0 and applied[8] == 20)
        assert(removed == xi.effectFlag.ERASABLE)
    end)

    it('does not remove a Dia flag when no Dia effect is present', function()
        local lamentation = require('scripts/actions/mobskills/lamentation')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local removed = false
        local mob = { getMainLvl = function() return 1 end }
        local target = {
            takeDamage = function() end,
            getStatusEffect = function() return nil end,
        }
        xi.mobskills.mobMagicalMove = function() return { damage = 1, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobStatusEffectMove = function() end

        assert(lamentation.onMobWeaponSkill(mob, target, {}, {}) == 1)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(removed == false)
    end)
end)
