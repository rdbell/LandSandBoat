describe('Floodlight mob skill', function()
    it('uses its Light plan and applies Blindness, Flash, and Silence only after processing', function()
        local light = require('scripts/actions/mobskills/floodlight'); local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}; local processed = false
        local mob = { getMainLvl = function() return 75 end }; local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,value) params=value; return { damage=123, attackType=xi.attackType.MAGICAL, damageType=xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return processed end; xi.mobskills.mobStatusEffectMove = function(_,_,...) effects[#effects+1]={...} end
        assert(light.onMobSkillCheck(target,mob,{})==0 and light.onMobWeaponSkill(mob,target,{},{})==123)
        assert(params.baseDamage==77 and params.fTP[1]==4.5 and params.fTP[2]==4.5 and params.fTP[3]==4.5 and params.element==xi.element.LIGHT and params.attackType==xi.attackType.MAGICAL and params.damageType==xi.damageType.LIGHT and params.shadowBehavior==xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage==nil and #effects==0)
        processed=true; light.onMobWeaponSkill(mob,target,{},{})
        xi.mobskills.mobMagicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove=move,process,status
        assert(damage[1]==123 and #effects==3 and effects[1][1]==xi.effect.BLINDNESS and effects[1][2]==15 and effects[1][3]==3 and effects[1][4]==120 and effects[2][1]==xi.effect.FLASH and effects[2][2]==0 and effects[2][3]==0 and effects[2][4]==20 and effects[3][1]==xi.effect.SILENCE and effects[3][2]==1 and effects[3][3]==0 and effects[3][4]==60)
    end)
end)
