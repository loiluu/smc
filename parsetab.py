
# parsetab.py
# This file is automatically generated. Do not edit.
_tabversion = '3.2'

_lr_method = 'LALR'

_lr_signature = '\xa0@!\xb5p\xf1`\x9dkZ~NX\xca\xb3\x8c'
    
_lr_action_items = {'AND':([3,4,7,9,10,11,12,13,14,15,17,18,27,28,31,32,33,34,35,36,37,39,46,47,48,49,50,51,],[-11,-8,24,-12,-26,-6,-9,-2,-10,-7,-25,24,-3,-1,-17,-14,-16,-24,-4,24,-15,-18,-13,-20,-21,-19,-23,-22,]),'REGEX':([23,],[34,]),'CONTAINS':([6,10,],[21,-26,]),'RPAREN':([3,4,9,10,11,12,13,14,15,17,18,27,28,29,31,32,33,34,35,36,37,39,46,47,48,49,50,51,],[-11,-8,-12,-26,-6,-9,-2,-10,-7,-25,28,-3,-1,38,-17,-14,-16,-24,-4,-5,-15,-18,-13,-20,-21,-19,-23,-22,]),'GTE':([38,],[45,]),'LTE':([38,],[42,]),'DECNUMBER':([30,41,42,43,44,45,],[39,47,48,49,50,51,]),'ALLSTRING':([0,2,16,24,25,],[1,1,1,1,1,]),'LT':([38,],[43,]),'NOT':([0,2,16,24,25,],[16,16,16,16,16,]),'LENGTH':([0,2,16,24,25,],[5,5,5,5,5,]),'GT':([38,],[41,]),'EQL':([6,8,10,38,],[22,26,-26,44,]),'LPAREN':([0,2,5,16,24,25,],[2,2,19,2,2,2,]),'IN':([6,10,],[23,-26,]),'VAR':([0,1,2,16,19,22,24,25,26,40,],[10,10,10,10,10,10,10,10,10,10,]),'STRSTR':([6,10,],[20,-26,]),'$end':([3,4,7,9,10,11,12,13,14,15,17,27,28,31,32,33,34,35,36,37,39,46,47,48,49,50,51,],[-11,-8,0,-12,-26,-6,-9,-2,-10,-7,-25,-3,-1,-17,-14,-16,-24,-4,-5,-15,-18,-13,-20,-21,-19,-23,-22,]),'OR':([3,4,7,9,10,11,12,13,14,15,17,18,27,28,31,32,33,34,35,36,37,39,46,47,48,49,50,51,],[-11,-8,25,-12,-26,-6,-9,-2,-10,-7,-25,25,-3,-1,-17,-14,-16,-24,-4,-5,-15,-18,-13,-20,-21,-19,-23,-22,]),'CONCAT':([10,32,],[-26,40,]),'NORMSTRING':([0,2,16,20,21,22,24,25,],[8,8,8,30,31,33,8,8,]),}

_lr_action = { }
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = { }
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'regex':([0,2,16,24,25,],[3,3,3,3,3,]),'allstring':([0,2,16,24,25,],[9,9,9,9,9,]),'strstr':([0,2,16,24,25,],[12,12,12,12,12,]),'constraint':([0,2,16,24,25,],[13,13,13,13,13,]),'contains':([0,2,16,24,25,],[4,4,4,4,4,]),'equal':([0,2,16,24,25,],[14,14,14,14,14,]),'length':([0,2,16,24,25,],[15,15,15,15,15,]),'var':([0,1,2,16,19,22,24,25,26,40,],[6,17,6,6,29,32,6,6,37,46,]),'formula':([0,2,16,24,25,],[7,18,27,35,36,]),'concat':([0,2,16,24,25,],[11,11,11,11,11,]),}

_lr_goto = { }
for _k, _v in _lr_goto_items.items():
   for _x,_y in zip(_v[0],_v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = { }
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> formula","S'",1,None,None,None),
  ('formula -> LPAREN formula RPAREN','formula',3,'p_formula_1','smc_parser.py',22),
  ('formula -> constraint','formula',1,'p_formula_2','smc_parser.py',26),
  ('formula -> NOT formula','formula',2,'p_formula_3','smc_parser.py',31),
  ('formula -> formula AND formula','formula',3,'p_formula_4','smc_parser.py',35),
  ('formula -> formula OR formula','formula',3,'p_formula_5','smc_parser.py',39),
  ('constraint -> concat','constraint',1,'p_constraint','smc_parser.py',43),
  ('constraint -> length','constraint',1,'p_constraint','smc_parser.py',44),
  ('constraint -> contains','constraint',1,'p_constraint','smc_parser.py',45),
  ('constraint -> strstr','constraint',1,'p_constraint','smc_parser.py',46),
  ('constraint -> equal','constraint',1,'p_constraint','smc_parser.py',47),
  ('constraint -> regex','constraint',1,'p_constraint','smc_parser.py',48),
  ('constraint -> allstring','constraint',1,'p_constraint','smc_parser.py',49),
  ('concat -> var EQL var CONCAT var','concat',5,'p_concat','smc_parser.py',55),
  ('equal -> var EQL var','equal',3,'p_equal','smc_parser.py',62),
  ('equal -> NORMSTRING EQL var','equal',3,'p_equal2','smc_parser.py',68),
  ('equal -> var EQL NORMSTRING','equal',3,'p_equal3','smc_parser.py',74),
  ('contains -> var CONTAINS NORMSTRING','contains',3,'p_contains','smc_parser.py',80),
  ('strstr -> var STRSTR NORMSTRING DECNUMBER','strstr',4,'p_strstr','smc_parser.py',86),
  ('length -> LENGTH LPAREN var RPAREN LT DECNUMBER','length',6,'p_length','smc_parser.py',92),
  ('length -> LENGTH LPAREN var RPAREN GT DECNUMBER','length',6,'p_length','smc_parser.py',93),
  ('length -> LENGTH LPAREN var RPAREN LTE DECNUMBER','length',6,'p_length','smc_parser.py',94),
  ('length -> LENGTH LPAREN var RPAREN GTE DECNUMBER','length',6,'p_length','smc_parser.py',95),
  ('length -> LENGTH LPAREN var RPAREN EQL DECNUMBER','length',6,'p_length','smc_parser.py',96),
  ('regex -> var IN REGEX','regex',3,'p_regex','smc_parser.py',102),
  ('allstring -> ALLSTRING var','allstring',2,'p_allstring','smc_parser.py',108),
  ('var -> VAR','var',1,'p_var','smc_parser.py',114),
]
