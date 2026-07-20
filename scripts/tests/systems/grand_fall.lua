describe('Grand Fall mob skill',function()
 it('uses its captured Water plan and damages only after processing',function()
  local fall=require('scripts/actions/mobskills/grand_fall');local move,process=xi.mobskills.mobMagicalMove,xi.mobskills.processDamage;local params,damage=nil,nil;local mob={getMainLvl=function()return 75 end};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobMagicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.MAGICAL,damageType=xi.damageType.WATER}end;xi.mobskills.processDamage=function()return false end
  assert(fall.onMobSkillCheck(target,mob,{})==0 and fall.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.fTP[1]==9 and params.fTP[2]==9 and params.fTP[3]==9 and params.element==xi.element.WATER and params.attackType==xi.attackType.MAGICAL and params.damageType==xi.damageType.WATER and params.shadowBehavior==xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage==nil);xi.mobskills.processDamage=function()return true end;fall.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobMagicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.MAGICAL and damage[4]==xi.damageType.WATER)
 end)
end)
