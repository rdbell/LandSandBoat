describe('Gravitic Horn mob skill',function()
 it('uses its target-HP Wind plan and damages only after processing',function()
  local horn=require('scripts/actions/mobskills/gravitic_horn');local move,process=xi.mobskills.mobMagicalMove,xi.mobskills.processDamage;local params,damage=nil,nil;local mob={};local target={getHP=function()return 500 end,takeDamage=function(_,...)damage={...}end};xi.mobskills.mobMagicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.MAGICAL,damageType=xi.damageType.WIND}end;xi.mobskills.processDamage=function()return false end
  assert(horn.onMobSkillCheck(target,mob,{})==0 and horn.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==500 and params.fTP[1]==.95 and params.fTP[2]==.95 and params.fTP[3]==.95 and params.element==xi.element.WIND and params.attackType==xi.attackType.MAGICAL and params.damageType==xi.damageType.WIND and params.shadowBehavior==xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage==nil);xi.mobskills.processDamage=function()return true end;horn.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobMagicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.MAGICAL and damage[4]==xi.damageType.WIND)
 end)
end)
