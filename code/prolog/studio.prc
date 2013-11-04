;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Copyright (C) 2002 Robert P. Wolf ;;
;;        ALL RIGHTS RESERVED        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


program studio #machine := "prolog.studio"


	[
		set_uap32_captions set_standard_captions set_edinburg_captions set_marseille_captions set_functional_captions
		auto_atoms scripted_atoms
		pr read readln
		write nl pp pt list
		file_writer import load consult batch file_reader reload
		remove_module create_module set_machine
		add_search_directory search_directories
		cd dir ls DIR ARGS args args_tail edit execute make_file make_directory erase erase_directory move copy
		CL cl addcl addcl0 DELCL OVERWRITE delcl delallcl lambda overwrite let
		create_atom search_atom preprocessor prompt
		+ ~ % < = > <=> <= =< >= => <> ! & | ^
		add sub mult div mod and or xor sum times less less_eq greater greater_eq max min
		abs cos degrad e exp log2 log10 ln log pi pow sin tan trunc
		timestamp
		is_atom is_integer is_double is_number is_var is_head is_text has_machine
		text_list text_term e32 query_stack object_counter
		exit command inner inner_addcl inner_call minimise maximise
		res not eq rres ures lazy random_cl explode TRY ONE PROBE SELECT APPEND LENGTH REVERSE ONLIST INLIST NODUP MAP
		sort divide
		WHILE FOR FOREVER forever IF
		CONSTANT VARIABLE ACCUMULATOR CLOSURE ARRAY var inc dec closure call BLOCK ALL ISALL isall isallr
		rnd rnd_control grnd series
		crack wait timeout enter signal semaphore msemaphore mutex accept select critical_section monitor task
		background foreground OPEN_EDITOR open_editor close_editor
		screen_coordinates
		open_keyboard open_oscilloscope
		get_volume_serial_number security_check

		computer-do-this define-this show
	]

#machine sum := "sum"
#machine + := "sum"
#machine add := "add"
#machine sub := "sub"
#machine times := "times"
#machine ~ := "times"
#machine mult := "mult"
#machine div := "div"
#machine mod := "mod"
#machine % := "mod"
#machine and := "and"
#machine & := "and"
#machine or := "or"
#machine | := "or"
#machine xor := "xor"
#machine ^ := "xor"

#machine abs := "abs"
#machine cos := "cos"
#machine degrad := "degrad"
#machine e := "e"
#machine exp := "exp"
#machine log2 := "log2"
#machine log10 := "log10"
#machine ln := "ln"
#machine log := "log"
#machine pi := "pi"
#machine pow := "pow"
#machine sin := "sin"
#machine tan := "tan"
#machine trunc := "trunc"

#machine timestamp := "timestamp"

#machine is_atom := "is_atom"
#machine is_integer := "is_integer"
#machine is_double := "is_double"
#machine is_number := "is_number"
#machine is_var := "is_var"
#machine is_head := "is_head"
#machine is_text := "is_text"
#machine has_machine := "has_machine"
#machine text_list := "text_list"
#machine text_term := "text_term"
#machine e32 := "e32"
#machine less := "less"
#machine less_eq := "less_eq"
#machine greater := "greater"
#machine greater_eq := "greater_eq"
#machine < := "less"
#machine <= := "less_eq"
#machine =< := "less_eq"
#machine > := "greater"
#machine >= := "greater_eq"
#machine => := "greater_eq"
#machine max := "max"
#machine min := "min"
#machine set_standard_captions := "set_standard_captions"
#machine set_uap32_captions := "set_uap32_captions"
#machine set_edinburg_captions := "set_edinburg_captions"
#machine set_marseille_captions := "set_marseille_captions"
#machine set_functional_captions := "set_functional_captions"
#machine auto_atoms := "auto_atoms"
#machine scripted_atoms := "scripted_atoms"
#machine pr := "pr"
#machine read := "read"
#machine readln := "readln"
#machine pp := "pp"
#machine pt := "pt"
#machine write := "write"
#machine nl := "nl"
#machine list := "list"
#machine CL := "CL"
#machine addcl := "addcl"
#machine addcl0 := "addcl0"
#machine DELCL := "delcl"
#machine OVERWRITE := "overwrite"
#machine create_atom := "create_atom"
#machine search_atom := "search_atom"
#machine preprocessor := "preprocessor"
#machine prompt := "prompt"
#machine query_stack := "query_stack"
#machine object_counter := "object_counter"
#machine crack := "crack"
#machine wait := "wait"
#machine timeout := "timeout"
#machine semaphore := "semaphore"
#machine msemaphore := "msemaphore"
#machine mutex := "mutex"
#machine rnd := "rnd"
#machine rnd_control := "rnd_control"
#machine series := "series"

#machine file_writer := "file_writer"
#machine file_reader := "file_reader"
#machine import := "import_loader"
#machine load := "load_loader"
#machine consult := "consult_loader"
#machine remove_module := "remove_module"
#machine create_module := "create_module"
#machine set_machine := "set_machine"
#machine add_search_directory := "add_search_directory"
#machine search_directories := "search_directories"
#machine cd := "cd"
#machine DIR := "DIR"
#machine ARGS := "ARGS"
#machine edit := "edit"
#machine make_directory := "make_directory"
#machine erase := "erase"
#machine erase_directory := "erase_directory"
#machine move := "move"
#machine copy := "copy"
#machine execute := "execute"

#machine CONSTANT := "CONSTANT"
#machine VARIABLE := "VARIABLE"
#machine ACCUMULATOR := "ACCUMULATOR"
#machine CLOSURE := "CLOSURE"
#machine ARRAY := "ARRAY"

#machine background := "background"
#machine foreground := "foreground"
#machine OPEN_EDITOR := "open_editor"
#machine close_editor := "close_editor"
#machine screen_coordinates := "screen_coordinates"

#machine get_volume_serial_number := "get_volume_serial_number"
#machine security_check := "security_check"

[[open_editor] [OPEN_EDITOR]]
[[open_editor *id] [OPEN_EDITOR *id]]
[[open_editor *x *y] [screen_coordinates *x *y] [OPEN_EDITOR]]
[[open_editor *id *x *y] [screen_coordinates *x *y] [OPEN_EDITOR *id]]
[[open_keyboard] [OPEN_EDITOR 1]]
[[open_keyboard *x *y] [screen_coordinates *x *y] [OPEN_EDITOR 1]]
[[open_oscilloscope] [OPEN_EDITOR 3]]
[[open_oscilloscope *x *y] [screen_coordinates *x *y] [OPEN_EDITOR 3]]


[[grnd : *parameters] [rnd : *parameters]]
[[grnd : *parameters] / [grnd : *parameters]]
[[eq *x *x]]
[[= *x *x]]
[[<> *x *x] / fail]
[[<> * *]]
[[! : *x] *x / fail]
[[! : *]]
[[not :*x] *x / fail]
[[not :*x]]
[[res :*x]:*x]
[[ONE :*o][res :*o]/]
[[ALL :*o][res : *o] fail]
[[ALL :*]]
[[TRY :*o]:*o]
[[TRY :*o]]
[[PROBE :*o][ONE :*o] fail]
[[PROBE :*o]]
[[SELECT] / fail]
[[SELECT *branch : *] : *branch]
[[SELECT * : *branches] / [SELECT : *branches]]
[[APPEND [] *l *l]]
[[APPEND [*head : *tail] *l [*head : *new]]/
	[APPEND *tail *l *new]]
[[REVERSE *l1 *l2] [REVERSE *l1 [] *l2]]
[[REVERSE [] *x *x]]
[[REVERSE [*head : *tail] *l0 *list] [REVERSE *tail [*head : *l0] *list]]
[[LENGTH [] 0]]
[[LENGTH [*head : *tail] *length] [LENGTH *tail *l] [sum *l 1 *length]]
[[ONLIST *x [*x : *]]]
[[ONLIST *x [* : *l]] [ONLIST *x *l]]
[[INLIST *l *x [*x : *l]]]
[[INLIST [*h : *l] *x [*h : *ll]] [INLIST *l *x *ll]]
[[NODUP [] []]/]
[[NODUP [*x : *t] *result] [ONLIST *x *t] / [NODUP *t *result]]
[[NODUP [*x : *t] [*x : *result]] / [NODUP *t *result]]
[[MAP [] [] []]]
[[MAP [[*x *y] : *xyt] [*x : *xt] [*y : *yt]] / [MAP *xyt *xt *yt]]
[[WHILE *condition : *call] [not not : *condition] / [PROBE : *call] / [WHILE *condition : *call]]
[[WHILE : *]]
[[FOREVER : *instructions] [PROBE : *instructions] / [FOREVER : *instructions]]
[[forever : *instructions] [res : *instructions] / [forever : *instructions]]
[[FOR *index *index *index *step : *call] [TRY : *call]/]
[[FOR *index *index *to *step : *call] [ONE : *call] fail]
[[FOR *index *from *to *step : *call]
	[add *from *step *next] /
	[FOR *index *next *to *step : *call]]
[[IF *condition *then] *condition / [TRY *then] /]
[[IF *condition *then] /]
[[IF *condition *then *else] *condition / [TRY *then] /]
[[IF *condition *then *else] [TRY *else] /]
[[var]]
[[var *var : *vars] [is_var *var] / [VARIABLE *var] / [var : *vars]]
[[var [*var *value] : *vars] / [VARIABLE *var *value] / [var : *vars]]
[[var *var : *vars] [VARIABLE *var] / [var : *vars]]
[[inc *var] [*var : *value] [add *value 1 *new] [*var *new]]
[[inc *var *inc] [*var : *value] [add *value *inc *new] [*var *new]]
[[dec *var] [*var : *value] [sub *value 1 *new] [*var *new]]
[[dec *var *dec] [*var : *value] [sub *value *dec *new] [*var *new]]
[[closure *atom *locals *parameters *term]
	[MAP *locals *local *values]
	[CLOSURE *atom : *values]
	[*atom : *atoms]
	[eq *local *atoms]
	[*atom [*term : *parameters]]]
[[call *atom : *parameters] [*atom *body : *parameters] : *body]
[[BLOCK *declarations : *body]
	[MAP *declarations *local *values]
	[create_atom *closure]
	[CLOSURE *closure : *values]
	[*closure : *atoms]
	[eq *local *atoms]
	[TRY : *body]]
[[ISALL *atom *template : *call]
	[res : *call]
	[*atom *template] fail]
[[ISALL :*]]
[[isall *list *template : *call]
	[create_atom *atom]
	[ACCUMULATOR *atom]
	[ISALL *atom *template : *call]
	[*atom : *list] /]
[[isallr *list *template : *call]
	[create_atom *atom]
	[ACCUMULATOR *atom]
	[ISALL *atom *template : *call]
	[*atom : *reversed_list]
	[REVERSE *reversed_list *list] /]

[[minimise *x *from *to] / [minimise *x *from *to 1]]
[[minimise *to *to *to *step]/]
[[minimise *from *from *to *step]]
[[minimise *x *from *to *step] [less *from *to] [add *from *step *next] / [minimise *x *next *to *step]]

[[maximise *x *from *to] / [maximise *x *from *to 1]]
[[maximise *from *from *from *step]/]
[[maximise *to *from *to *step]]
[[maximise *x *from *to *step] [less *from *to] [sub *to *step *next] / [maximise *x *from *next *step]]

[[lazy]]
[[lazy : *commands]
	[INLIST *next *command *commands]
	*command
	[lazy : *next]
]
[[rres [*atom : *parameters]]
	[CL *atom *clauses]
	[series 0 *clauses 1 *serie]
	/ [random_cl *serie *atom : *parameters]
]
[[ures [*atom : *parameters]]
	[isall *probabilities *ind [cl [[*atom *ind :*] :*]]]
	[LENGTH *probabilities *length]
	[sub *length 1 *next]
	[explode *probabilities *next *full_list]
	[series *full_list *permutated]
	/ [random_cl *permutated *atom : *parameters]
]
[[random_cl [*h : *t] *atom : *parameters]
	[CL *h *atom [[*atom : *parameters] : *body]]
	: *body
]
[[random_cl [* : *t] : *call] / [random_cl *t : *call]]
[[explode [] * []]/]
[[explode [*head : *tail] *ind [*ind : *subtail]]
	[less 0 *head]
	/ [sub *head 1 *next] / [explode [*next : *tail] *ind *subtail]
]
[[explode [* : *tail] *ind *subtail]
	[sub *ind 1 *next]
	/ [explode *tail *next *subtail]
]

[[divide *less *x [] *lt *lt *gt *gt]/]
[[divide *less *x [*h : *t] *lt *lta [*h : *gt] *gta] [*less *x *h] / [divide *less *x *t *lt *lta *gt *gta]]
[[divide *less *x [*h : *t] [*h : *lt] *lta *gt *gta] / [divide *less *x *t *lt *lta *gt *gta]]

[[sort *less [] *gs *gs]/]
[[sort *less [*h : *t] *gts *gtsa]
	[divide *less *h *t *lt *lta *gt *gta]/
	[sort *less *lt *gts [*h : *gs]]/
	[sort *less *gt *gs *gtsa]/
]

[[<=> *less [] *gs *gs]/]
[[<=> *less [*h : *t] *gts *gtsa]
	[divide *less *h *t *lt *lta *gt *gta] /
	[<=> *less *lt *gts [*h : *gs]] /
	[<=> *less *gt *gs *gtsa] /
]





[[cl *x]/[cl 0 *y *x]]
[[cl *x *y]/[cl 0 *x *y]]
[[cl *x *x [[*a:*b]:*c]][CL *x *a [[*a:*b]:*c]]]
[[cl *x *y [[*a:*b]:*c]][add *x 1 *x2]/[CL *x2 *a *X]
	[cl *x2 *y [[*a:*b]:*c]]]

[[lambda *atom *parameters : *body] [create_atom *atom] [addcl [[*atom : *parameters] : *body]]]
[[lambda *atom *parameters : *body] [addcl [[*atom : *parameters] : *body]]]

[[delcl [[*a:*b]:*c]][cl *x [[*a:*b]:*c]][DELCL *x *a]]

[[delallcl *a] [DELCL 0 *a] / [delallcl *a]]
[[delallcl *a]]

[[overwrite [[*a:*b]:*c] [[*a:*h]:*i]][cl *x [[*a:*b]:*c]]
	[OVERWRITE *x [[*a:*h]:*i]]]
[[let *name *selector : *value]
	[overwrite [[*name *selector : *] : *] [[*name *selector : *value]]]]


; multithreading and synchronization

[[monitor *atom]
	[is_atom *atom]
	[semaphore *s]
	[addcl [[*atom *mode : *commands] [*s *mode] [TRY : *commands] [*s signal] [wait 0]]]
	]

[[critical_section [*atom : *parameters] : *commands]
	[is_atom *atom]
	[semaphore *s]
	[addcl [[*atom : *parameters] [*s wait] [TRY : *commands] [*s signal] [wait 0]]]
	]

;;;;;;;;;;;;;;
; RENDEZVOUS ;
;;;;;;;;;;;;;;
; THREAD: .... [*task enter atom : *parameters]
; TASK:   .... [*task accept atom [*parameters:...] : *body]
;     OR: .... [SELECT
;                      [.... conditions ..... [*task select atom [*parameters:...] : *body] ..... body .... ]
;                      [.... conditions ..... [*task select sonda [*parameters:...] : *body] ..... body .... ]
;                      [.... conditions ..... [*task select extra [*parameters:...] : *body] ..... body .... ]
;                      [[*task wait]]     ; this prevents active pooling by a waiting for a semaphore activated by either [*task enter....] or [*task signal]
;              ]
;;;;;;;;;;;;;;

[[task accept *atom *trigger []]]
[[task accept *atom *trigger [*entry : *entries]]
	[semaphore *enter 0] [semaphore *accept 0] [semaphore *complete 0]
	[var *data]
	[addcl [[*atom accept *entry : *body]
				[*enter wait]                                  ; wait for the enter trigger
				[*data *body]                                  ; transfer parameters and body
				[*accept signal]                               ; indicate transfer complete
				[*complete wait]                               ; wait for completion on the other side
			]
	]
	[addcl [[*atom enter *entry : *parameters]
				[*enter signal]                                ; trigger entry
				[*trigger signal]                              ; just in case if the task is suspended on default wait
				[*accept wait]                                 ; wait for parameters and body
				[*data : *d]                                   ; get them
				[TRY [eq [*parameters : *body] *d] : *body]    ; and perform
				[*complete signal]                             ; indicate the completion
			]
	]
	[addcl [[*atom select *entry : *body]
				[*enter enter]                                 ; main trigger has already been passed, so just wait for the particular trigger
				[*data *body]                                  ; transfer parameters and body
				[*accept signal]                               ; indicate transfer complete
				[*complete wait]                               ; wait for completion on the other side
			]
	]
	/ [task accept *atom *trigger *entries]
]
[[task *atom *entries : *commands]
	[create_atom *atom] [semaphore *trigger 0]
	[task accept *atom *trigger *entries]
	[addcl [[*atom wait]   [*trigger wait]]   ]                ; stop actively pooling until trigger
	[addcl [[*atom signal] [*trigger signal]] ]                ; re-activate active pooling
	[crack : *commands]
]	

; command processor

[[exit]]

;[[inner [[*atom : *] : *] []]]
[[inner [* : *] []]]
[[inner *x *y] [pr *y]]
[[inner [*x *y]] [pr *x] [inner *x *y] /]
[[inner [*x *y]] [readln *x] fail]

[[inner_addcl [[*atom : *t1] : *t2]]
	[preprocessor *preprocessor]
	[is_atom *preprocessor]
	/ [*preprocessor *t2 *t3] [addcl [[*atom : *t1] : *t3]]]
[[inner_addcl *cl] [addcl *cl]]

[[inner_call *command] [preprocessor *preprocessor] [is_atom *preprocessor] / [*preprocessor [*command] *c] : *c]
[[inner_call *command] *command]

[[command [exit]]]
[[command [[exit]]]]
[[command *x] [inner [*y *z]] [command *y *z] / [command [*y : *z]]]
[[command *x] [write "Doesn't work"] [nl] / [command *x]]
[[command] [command [command]]]
[[command [[*atom : *t1] : *t2] []] [inner_addcl [[*atom : *t1] :*t2]]]
[[command [[*atom : *t1] : *t2] []] [write "Can not add clause: " [[[*atom : *t1] : *t2]] "\n"] / fail]
[[command [*x : *y] []] / [inner_call [*x : *y]]]
[[command *x *y] [inner_call [*x : *y]]]


[[batch *text] [create_atom *atom] [file_reader *atom *text] / [batch [batch] *atom]]
[[batch [exit] *reader][*reader]]
[[batch *x *reader][*reader *head]/[batch *x *reader *head]] ;[*reader *tail][*head :*tail] / [batch [*head :*tail] *reader]]
[[batch *x *reader][*reader] / fail]
[[batch *x *reader *head] [eq *head [[* : *] : *]] / [inner_addcl *head] / [batch *head *reader]]
[[batch *x *reader *head] [is_atom *head] / [*reader *tail] [inner_call [*head : *tail]] / [batch [*head : *tail] *reader]]
[[batch *x *reader *head] / [inner_call *head] / [batch *head *reader]]

[[reload] [list :*x] [eq *x [*user *loader :*tail]] [add *loader ".prc" *name] [load *name]]

[[make_file *file_name] [create_atom *atom] [file_writer *atom *file_name] [*atom]]

[[dir] / [DIR "*.*" : *files] [dir : *files]]
[[dir *location] [is_text *location] / [DIR *location : *files] [dir : *files]]
[[dir *result] [is_var *result] / [DIR "*.*" : *result]]
[[dir *location *result] [is_text *location] [is_var *result] / [DIR *location : *result]]
[[dir [*file : *files]] [write "	" *file] [nl] / [dir *files]]
[[dir []] /]
[[dir *directories *files]
	[write "Directories:\n"]
	[dir *directories]
	[write "Files:\n"]
	[dir *files]
	]
[[ls : *command] / [dir : *command]]

[[args : *args] [ARGS : *texts] / [args_tail *texts *args]]
[[args_tail [] []] /]
[[args_tail [*t1 : *t] *a] [text_term *t1 : *b] [APPEND *b *c *a] / [args_tail *t *c]]

; demo
[[computer-do-this : *x] : *x]
[[define-this : *x] [addcl *x]]
[[show] [nl] /]
[[show *text : *show] [is_text *text] [write *text] / [show : *show]]
[[show *x : *show] [pp *x] / [show : *show]]

protect [
	not
	exit command inner inner_addcl inner_call random_cl explode
	APPEND REVERSE LENGTH ONLIST NODUP
	sort divide
	]
private [inner inner_addcl inner_call random_cl explode divide args_tail]


end.
