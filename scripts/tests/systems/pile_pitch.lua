require('scripts/actions/mobskills/pile_pitch')

describe('Pile Pitch mob skill', function()
    it('uses its percent-HP physical plan and applies Bind plus enmity reset after processing', function()
        local skill = require('scripts/actions/mobskills/pile_pitch')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, enmityTarget, takeOpts = nil, nil, nil, nil, nil
        local mob = {
            resetEnmity = function(_, target) enmityTarget = target end,
        }
        local target = {
            getHP = function() return 1000 end,
            takeDamage = function(_, dmg, attacker, at, dt, opts)
                damage = { dmg, attacker, at, dt }
                takeOpts = opts
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 900, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            statusParams = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0 and skill.onMobWeaponSkill(mob, target, {}, {}) == 900)
        assert(params.baseDamage == 1000 and params.numHits == 1 and params.fTP[1] == 0.90)
        assert(params.skipFSTR and params.skipPDIF and params.skipParry and params.skipGuard and params.skipBlock)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and statusParams == nil and enmityTarget == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 900 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(takeOpts and takeOpts.breakBind == false)
        assert(statusParams[3] == xi.effect.BIND and statusParams[4] == 1 and statusParams[5] == 0 and statusParams[6] == 30)
        assert(enmityTarget == target)
    end)
end)
