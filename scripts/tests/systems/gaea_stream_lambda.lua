describe('Gaea Stream: Lambda mob skill',function()
 it('requires animation sub two and uses its slashing plan only after processing',function()
  local gaea=require('scripts/actions/mobskills/gaea_stream_lambda');local move,process=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage;local params,damage=nil,nil;local animation=2;local mob={getAnimationSub=function()return animation end,getWeaponDmg=function()return 77 end};local target={takeDamage=function(_,...)damage={...}end}
  assert(gaea.onMobSkillCheck(target,mob,{})==0);animation=0;assert(gaea.onMobSkillCheck(target,mob,{})==1);animation=2
  xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.SLASHING}end;xi.mobskills.processDamage=function()return false end
  assert(gaea.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==2.25 and params.fTP[2]==2.25 and params.fTP[3]==2.25 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.SLASHING and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_2 and damage==nil)
  xi.mobskills.processDamage=function()return true end;gaea.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.SLASHING)
 end)
end)
