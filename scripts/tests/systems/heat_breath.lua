describe('Heat Breath mob skill',function()
 it('uses its Fire breath plan and damages only after processing',function()
  local breath=require('scripts/actions/mobskills/heat_breath');local move,process=xi.mobskills.mobBreathMove,xi.mobskills.processDamage;local params,damage=nil,nil;local mob={};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobBreathMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.BREATH,damageType=xi.damageType.FIRE}end;xi.mobskills.processDamage=function()return false end
  assert(breath.onMobSkillCheck(target,mob,{})==0 and breath.onMobWeaponSkill(mob,target,{},{})==123);assert(params.percentMultipier==.125 and params.damageCap==500 and params.bonusDamage==0 and params.mAccuracyBonus[1]==0 and params.mAccuracyBonus[2]==0 and params.mAccuracyBonus[3]==0 and params.resistStat==xi.mod.INT and params.element==xi.element.FIRE and params.attackType==xi.attackType.BREATH and params.damageType==xi.damageType.FIRE and params.shadowBehavior==xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage==nil);xi.mobskills.processDamage=function()return true end;breath.onMobWeaponSkill(mob,target,{})
  xi.mobskills.mobBreathMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.BREATH and damage[4]==xi.damageType.FIRE)
 end)
end)
