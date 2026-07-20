describe('Frost Breath mob skill', function()
    it('requires zone 5 and applies Ice breath damage and Paralysis only after processing', function()
        local breath=require('scripts/actions/mobskills/frost_breath');local move,process,status=xi.mobskills.mobBreathMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove;local params,damage,paralysis=nil,nil,nil
        local mob={getZoneID=function()return 5 end};local outside={getZoneID=function()return 1 end};local target={takeDamage=function(_,...)damage={...}end}
        xi.mobskills.mobBreathMove=function(_,_,_,_,value)params=value;return {damage=123,attackType=xi.attackType.BREATH,damageType=xi.damageType.ICE}end;xi.mobskills.processDamage=function()return false end;xi.mobskills.mobStatusEffectMove=function(...)paralysis={...}end
        assert(breath.onMobSkillCheck(target,mob,{})==0 and breath.onMobSkillCheck(target,outside,{})==1 and breath.onMobWeaponSkill(mob,target,{},{})==123)
        assert(params.percentMultipier==.083 and params.damageCap==500 and params.bonusDamage==0 and params.mAccuracyBonus[1]==0 and params.mAccuracyBonus[2]==0 and params.mAccuracyBonus[3]==0 and params.resistStat==xi.mod.INT and params.element==xi.element.ICE and params.attackType==xi.attackType.BREATH and params.damageType==xi.damageType.ICE and params.shadowBehavior==xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage==nil and paralysis==nil)
        xi.mobskills.processDamage=function()return true end;breath.onMobWeaponSkill(mob,target,{},{})
        xi.mobskills.mobBreathMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove=move,process,status
        assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.BREATH and damage[4]==xi.damageType.ICE and paralysis[1]==mob and paralysis[2]==target and paralysis[3]==xi.effect.PARALYSIS and paralysis[4]==50 and paralysis[5]==0 and paralysis[6]==180)
    end)
end)
