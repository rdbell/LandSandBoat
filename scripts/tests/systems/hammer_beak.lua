describe('Hammer Beak mob skill',function()
 it('uses its Zizzy Zillah fTP override and damages only after processing',function()
  local beak=require('scripts/actions/mobskills/hammer_beak');local move,process=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage;local params,damage=nil,nil;local pool=0;local mob={getWeaponDmg=function()return 77 end,getPool=function()return pool end};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.SLASHING}end;xi.mobskills.processDamage=function()return false end
  assert(beak.onMobSkillCheck(target,mob,{})==0 and beak.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==3 and params.fTP[2]==3 and params.fTP[3]==3 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.SLASHING and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage==nil);pool=xi.mobPool.ZIZZY_ZILLAH;beak.onMobWeaponSkill(mob,target,{},{})
  assert(params.fTP[1]==3.25 and params.fTP[2]==3.25 and params.fTP[3]==3.25);xi.mobskills.processDamage=function()return true end;beak.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.SLASHING)
 end)
end)
