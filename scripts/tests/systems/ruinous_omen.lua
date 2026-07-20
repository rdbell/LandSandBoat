require('scripts/actions/mobskills/ruinous_omen')
describe('Ruinous Omen mob skill', function()
    it('uses dark magical plan and damages only after processing', function()
        local omen = require('scripts/actions/mobskills/ruinous_omen')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        assert(omen.onMobSkillCheck(target, mob, {}) == 0 and omen.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 10 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        omen.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
