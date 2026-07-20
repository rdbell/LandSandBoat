require('scripts/actions/mobskills/shock_wave_behemoth')
describe('Shock Wave Behemoth mob skill', function()
    it('rejects behind targets and uses wind magical plan', function()
        local wave = require('scripts/actions/mobskills/shock_wave_behemoth')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, behind = nil, nil, true
        local mob = { getMainLvl = function() return 60 end }
        local target = {
            isBehind = function(_, m, angle) assert(angle == 48); return behind end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(wave.onMobSkillCheck(target, mob, {}) == 1)
        behind = false; assert(wave.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(wave.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.baseDamage == 62 and params.fTP[1] == 0.6 and params.element == xi.element.WIND and damage == nil)
        xi.mobskills.processDamage = function() return true end
        wave.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 40)
    end)
end)
