describe('Frypan mob skill',function()
 it('uses its blunt plan and stuns only after damage processing',function()
  local fry=require('scripts/actions/mobskills/frypan');local move,process,status=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove;local params,damage,stun=nil,nil,nil;local mob={getWeaponDmg=function()return 77 end};local target={takeDamage=function(_,...)damage={...}end}
  xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.BLUNT}end;xi.mobskills.processDamage=function()return false end;xi.mobskills.mobStatusEffectMove=function(...)stun={...}end
  assert(fry.onMobSkillCheck(target,mob,{})==0 and fry.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==1 and params.fTP[2]==1 and params.fTP[3]==1 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.BLUNT and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage==nil and stun==nil)
  xi.mobskills.processDamage=function()return true end;fry.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove=move,process,status;assert(damage[1]==123 and stun[3]==xi.effect.STUN and stun[4]==1 and stun[5]==0 and stun[6]==4)
 end)
end)
