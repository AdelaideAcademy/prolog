///////////////////////////////////////////////////////////////////////////////////
//                       Copyright (C) 2004 Robert P. Wolf                       //
//                                                                               //
// Permission is hereby granted, free of charge, to any person obtaining a copy  //
// of this software and associated documentation files (the "Software"), to deal //
// in the Software without restriction, including without limitation the rights  //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell     //
// copies of the Software, and to permit persons to whom the Software is         //
// furnished to do so, subject to the following conditions:                      //
//                                                                               //
// The above copyright notice and this permission notice shall be included in    //
// all copies or substantial portions of the Software.                           //
//                                                                               //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     //
// THE SOFTWARE.                                                                 //
///////////////////////////////////////////////////////////////////////////////////

#include "prolog_studio.h"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

class pp : public PrologNativeCode {
public:
	PrologRoot * root;
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		AREA area;
		while (parameters -> isPair ()) {
			root -> getValue (parameters -> getLeft (), area, 0);
			root -> print (area);
			parameters = parameters -> getRight ();
		}
		return true;
	}
	pp (PrologRoot * root) {this -> root = root;}
};

class pt : public PrologNativeCode {
public :
	PrologRoot * root;
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		AREA area;
		while (parameters -> isPair ()) {
			root -> getTrueValue (parameters -> getLeft (), area, 0);
			root -> print (area);
			parameters = parameters -> getRight ();
		}
		return true;
	}
	pt (PrologRoot * root) {this -> root = root;}
};

class write : public PrologNativeCode {
public:
	PrologRoot * root;
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		AREA area;
		PrologElement * el;
		while (parameters -> isPair ()) {
			el = parameters -> getLeft ();
			if (el -> isText ()) root -> print (el -> getText());
			if (el -> isInteger ()) root -> print_character (el -> getInteger ());
			while (el -> isPair ()) {
				root -> getValue (el -> getLeft (), area, 0);
				root -> print (area);
				el = el -> getRight ();
			}
			parameters = parameters -> getRight ();
		}
		return true;
	}
	write (PrologRoot * root) {this -> root = root;}
};

class nl : public PrologNativeCode {
public:
	PrologRoot * root;
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {root -> print (root -> new_line_caption); return true;}
	nl (PrologRoot * root) {this -> root = root;}
};

void standard_in_reader :: message (char * text) {root -> print (text); root -> print (root -> new_line_caption);}
void standard_in_reader :: message_v (char * text, char * variable) {root -> message (text, variable);}
int standard_in_reader :: move_z (void) {return root -> get_character ();}

class read : public PrologNativeCode {
public:
	standard_in_reader * stdr;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		AREA area;
		stdr -> getString (area, 0);
		parameters -> setText (area);
		return true;
	}
	read (standard_in_reader * stdr) {this -> stdr = stdr;}
};

class readln : public PrologNativeCode {
public:
	standard_in_reader * stdr;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		AREA area;
		stdr -> readln (area, 0);
		parameters -> setText (area);
		return true;
	}
	readln (standard_in_reader * stdr) {this -> stdr = stdr;}
};

class pr : public PrologNativeCode {
public:
	standard_in_reader * stdr;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * dup = stdr -> readElement ();
		if (dup != NULL) {
			parameters -> setLeft (dup);
			return true;
		}
		return false;
	}
	pr (standard_in_reader * stdr) {this -> stdr = stdr;}
};

class list : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		AREA area;
		int area_ind;
		if (parameters -> isEarth ()) {
			// directory list
			root -> list (area, 0);
			root -> print (area); root -> print (root -> new_line_caption);
			return true;
		}
		if (parameters -> isVar ()) {
			// directory drop
			PrologDirectory * dr = root -> root;
			while (dr != NULL) {
				parameters -> setPair (root -> text (dr -> directoryName), root -> earth ());
				parameters = parameters -> getRight ();
				dr = dr -> next;
			}
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * ret = parameters -> getRight ();
		parameters = parameters -> getLeft ();
		if (parameters -> isVar ()) {
			// directory drop list
			root -> list (area, 0);
			parameters -> setText (area);
			return true;
		}
		if (parameters -> isAtom ()) {
			PrologAtom * atom = parameters -> getAtom ();
			if (atom -> getMachine () != NULL) {
				area_ind = area_cat (area, 0, atom -> name ());
				area_ind = area_cat (area, area_ind, " ");
				area_ind = area_cat (area, area_ind, root -> if_atom_caption);
				area_ind = area_cat (area, area_ind, " ");
				area_ind = area_cat (area, area_ind, root -> machine_caption);
				if (ret -> isVar ()) {
					// machine clause drop
					ret -> setPair (root -> text (area), root -> earth ());
					return true;
				}
				// machine clause drop list / machine clause list
				area_ind = area_cat (area, area_ind, root -> new_line_caption);
				if (ret -> isPair ()) ret -> getLeft () -> setText (area); else root -> print (area);
				return true;
			}
			PrologElement * clausa = atom -> firstClause;
			PrologElement * dup = NULL;
			if (ret -> isVar ()) {
				// clause drop
				if (clausa == NULL) {ret -> setEarth (); return true;}
				while (clausa != NULL) {
					dup = clausa -> duplicate ();
					dup -> getLeft () -> getLeft () -> setAtom (atom);
					ret -> setPair (dup, root -> earth ());
					ret = ret -> getRight ();
					clausa = (PrologElement *) clausa -> getLeft () -> getLeft () -> getHead ();
				}
				return true;
			}
			area_ind = area_cat (area, 0, "");
			while (clausa != NULL) {
				dup = clausa -> duplicate ();
				dup -> getLeft () -> getLeft () -> setAtom (atom);
				area_ind = root -> getValue (dup, area, area_ind);
				delete dup;
				area_ind = area_cat (area, area_ind, root -> new_line_caption);
				clausa = (PrologElement *) clausa -> getLeft () -> getLeft () -> getHead ();
			}
			// clause drop list / clause list
			if (ret -> isPair ()) ret -> getLeft () -> setText (area); else root -> print (area);
			return true;
		}
		if (parameters -> isText ()) {
			if (ret -> isVar ()) {
				// atom drop
				PrologDirectory * dr = root -> searchDirectory (parameters -> getText ());
				if (dr == NULL) return false;
				PrologAtom * atom = dr -> firstAtom;
				ret -> setEarth ();
				while (atom != NULL) {
					ret -> setPair (root -> atom (atom), root -> earth ());
					ret = ret -> getRight ();
					atom = atom -> next;
				}
				return true;
			}
			area_ind = root -> list (parameters -> getText (), area, 0);
			// atom drop list / atom list
			if (ret -> isPair ()) ret -> getLeft () -> setText (area); else root -> print (area);
		}
		return true;
	}
	list (PrologRoot * root) {this -> root = root;}
};

class set_uap32_captions : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_uap32_captions ();
		return true;
	}
	set_uap32_captions (PrologRoot * root) {this -> root = root;}
};

class set_standard_captions : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_standard_captions ();
		return true;
	}
	set_standard_captions (PrologRoot * root) {this -> root = root;}
};

class set_edinburg_captions : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_edinburg_captions ();
		return true;
	}
	set_edinburg_captions (PrologRoot * root) {this -> root = root;}
};

class set_marseille_captions : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_marseille_captions ();
		return true;
	}
	set_marseille_captions (PrologRoot * root) {this -> root = root;}
};

class set_functional_captions : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_functional_captions ();
		return true;
	}
	set_functional_captions (PrologRoot * root) {this -> root = root;}
};

class auto_atoms : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_auto_atoms ();
		return true;
	}
	auto_atoms (PrologRoot * root) {this -> root = root;}
};

class scripted_atoms : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		root -> set_scripted_atoms ();
		return true;
	}
	scripted_atoms (PrologRoot * root) {this -> root = root;}
};

class CL : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		if (parameters -> getLeft () -> isAtom ()) {
			PrologAtom * atom = parameters -> getLeft () -> getAtom ();
			parameters = parameters -> getRight ();
			PrologElement * e_counter = parameters;
			if (parameters -> isPair ()) e_counter = parameters -> getLeft ();
			if (e_counter -> isVar () || ! parameters -> isPair ()) {
				PrologElement * clause = atom -> firstClause;
				int counter = 0;
				while (clause != NULL) {
					counter++;
					clause = (PrologElement *) clause -> getLeft () -> getLeft () -> getHead ();
				}
				e_counter -> setInteger (counter);
				return true;
			}
			if (! e_counter -> isInteger ()) return false;
			if (! parameters -> isPair ()) return false;
			PrologElement * e_clause = parameters;
			parameters = parameters -> getRight ();
			if (parameters -> isEarth ()) {
				PrologElement * clause = atom -> firstClause;
				int counter = 0;
				while (clause != NULL) {
					counter++;
					clause = (PrologElement *) clause -> getLeft () -> getLeft () -> getHead ();
				}
				return e_counter -> getInteger () == counter;
			}
			PrologElement * dup = atom -> firstClause;
			if (dup == NULL) return false;
			int ind = e_counter -> getInteger ();
			while (ind > 0) {
				dup = (PrologElement *) dup -> getLeft () -> getLeft () -> getHead ();
				if (dup == NULL) return false;
				ind--;
			}
			dup = dup -> duplicate ();
			dup -> getLeft () -> getLeft () -> setAtom (atom);
			if (parameters -> isVar ()) {e_clause -> setRight (dup); return true;}
			parameters -> setPair ();
			parameters -> setLeft (dup);
			return true;
		}
		if (! parameters -> getLeft () -> isInteger ()) return false;
		int ind = parameters -> getLeft () -> getInteger ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		if (! parameters -> getLeft () -> isAtom ()) return false;
		PrologAtom * atom = parameters -> getLeft () -> getAtom ();
		PrologElement * e_clause = parameters;
		parameters = parameters -> getRight ();
		PrologElement * dup = atom -> firstClause;
		if (dup == NULL) return false;
		while (ind > 0) {
			dup = (PrologElement *) dup -> getLeft () -> getLeft () -> getHead ();
			if (dup == NULL) return false;
			ind--;
		}
		dup = dup -> duplicate ();
		dup -> getLeft () -> getLeft () -> setAtom (atom);
		if (parameters -> isVar ()) {e_clause -> setRight (dup); return true;}
		parameters -> setPair ();
		parameters -> setLeft (dup);
		return true;
	}
	CL (PrologRoot * root) {}
};

class delcl : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		PrologAtom * atom = 0;
		int ind = -1;
		while (parameters -> isPair ()) {
			PrologElement * e = parameters -> getLeft ();
			if (e -> isAtom ()) atom = e -> getAtom ();
			else if (e -> isInteger ()) ind = e -> getInteger ();
			else return false;
			parameters = parameters -> getRight ();
		}
		if (atom == 0 || ind < 0) return false;
		if (atom -> Protected) return false;
		PrologElement * dup = atom -> firstClause;
		if (dup == NULL) return false;
		if (ind == 0) {
			atom -> firstClause = (PrologElement *) dup -> getLeft () -> getLeft () -> getHead ();
			delete dup;
			return true;
		}
		ind--;
		while (ind > 0) {
			dup = (PrologElement *) dup -> getLeft () -> getLeft () -> getHead ();
			if (dup == NULL) return false;
			ind--;
		}
		dup = dup -> getLeft () -> getLeft ();
		PrologElement * next = (PrologElement *) dup -> getHead ();
		if (next == NULL) return false;
		dup -> setHead ((PrologElement *) next -> getLeft () -> getLeft () -> getHead ());
		delete next;
		return true;
	}
	delcl (PrologRoot * root) {}
};

class overwrite : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		if (! parameters -> getLeft () -> isInteger ()) return false;
		int ind = parameters -> getLeft () -> getInteger ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isPair ()) return false;
		if (! parameters -> getLeft () -> isPair ()) return false;
		if (! parameters -> getLeft () -> getLeft () -> isAtom ()) return false;
		PrologAtom * atom = parameters -> getLeft () -> getLeft () -> getAtom ();
		if (atom -> Protected) return false;
		PrologElement * dup = atom -> firstClause;
		if (dup == NULL) return false;
		while (ind > 0) {
			dup = (PrologElement *) dup -> getLeft () -> getLeft () -> getHead ();
			if (dup == NULL) return false;
			ind--;
		}
		dup -> getLeft () -> setRight (parameters -> getLeft () -> getRight () -> duplicate ());
		dup -> setRight (parameters -> getRight () -> duplicate ());
		return true;
	}
	overwrite (PrologRoot * root) {}
};

class addcl0 : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * dup = parameters -> getLeft () -> duplicate ();
		if (root -> attachClause (dup, 0) == 0) return true;
		delete dup;
		return false;
	}
	addcl0 (PrologRoot * root) {this -> root = root;}
};

class addcl : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * dup = parameters -> getLeft () -> duplicate ();
		parameters = parameters -> getRight ();
		if (parameters -> isEarth ()) {if (root -> attachClause (dup) == 0) return true;}
		if (parameters -> isPair ()) {
			parameters = parameters -> getLeft ();
			if (parameters -> isInteger ()) {
				if (root -> attachClause (dup, parameters -> getInteger ()) == 0) return true;
			}
		}
		delete dup;
		return false;
	}
	addcl (PrologRoot * root) {this -> root = root;}
};

class create_atom : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * left = parameters -> getLeft ();
		if (left -> isAtom ()) return true;
		if (left -> isVar ()) {left -> setAtom (new PrologAtom ()); return true;}
		if (! left -> isText ()) return false;
		parameters = parameters -> getRight ();
		PrologAtom * atom;
		if (parameters -> isPair ()) {
			atom = new PrologAtom (left -> getText ());
			if (atom == NULL) return false;
			parameters -> getLeft () -> setAtom (atom);
			return true;
		}
		root -> createAtom (left -> getText ());
		return true;
	}
	create_atom (PrologRoot * root) {this -> root = root;}
};

class search_atom : public PrologNativeCode {
public:
	PrologRoot * root;
	bool sub_code (PrologElement * parameters, char * name) {
		PrologAtom * atom = root -> search (name);
		if (atom == 0) return false;
		parameters -> setAtom (atom);
		return true;
	}
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * name = parameters -> getLeft (); if (! name -> isText ()) return false; parameters = parameters -> getRight ();
		if (parameters -> isVar ()) return sub_code (parameters, name -> getText ());
		if (! parameters -> isPair ()) return false;
		PrologElement * e = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (e -> isVar ()) return sub_code (e, name -> getText ());
		if (e -> isText ()) {
			PrologDirectory * dir = root -> searchDirectory (name -> getText ());
			if (dir == 0) return false;
			PrologAtom * atom = dir -> searchAtom (e -> getText ());
			if (atom == 0) return false;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			parameters -> setAtom (atom);
			return true;
		}
		return false;
	}
	search_atom (PrologRoot * root) {this -> root = root;}
};

class preprocessor : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		PrologAtom * preprocessor = root -> getPreprocessor ();
		if (parameters -> isEarth ()) {root -> setPreprocessor (NULL); return true;}
		if (! parameters -> isPair ()) {
			if (preprocessor != NULL) parameters -> setAtom (preprocessor);
			else parameters -> setEarth ();
			return true;
		}
		parameters = parameters -> getLeft ();
		if (parameters -> isAtom ()) {root -> setPreprocessor (parameters -> getAtom ()); return true;}
		if (parameters -> isEarth ()) {root -> setPreprocessor (NULL); return true;}
		if (parameters -> isVar ()) {
			if (preprocessor != NULL) parameters -> setAtom (preprocessor);
			else parameters -> setEarth ();
			return true;
		}
		return false;
	}
	preprocessor (PrologRoot * root) {this -> root = root;}
};

class prompt : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (root -> getCommander () == NULL) return false;
		if (! parameters -> isPair ()) {
			char * current_prompt = root -> getCommander () -> getPrompt ();
			if (current_prompt == NULL) return false;
			parameters -> setText (current_prompt);
			return true;
		}
		parameters = parameters -> getLeft ();
		if (parameters -> isText ()) {root -> getCommander () -> setPrompt (parameters -> getText ()); return true;}
		if (parameters -> isVar ()) {
			char * current_prompt = root -> getCommander () -> getPrompt ();
			if (current_prompt == NULL) return false;
			parameters -> setText (current_prompt);
			return true;
		}
		return false;
	}
	prompt (PrologRoot * root) {this -> root = root;}
};

class query_stack : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		PrologQuery * query = resolution -> getQuery ();
		while (query != NULL) {
			drop_element (root, query -> query);
			query = query -> stack;
		}
		return true;
	}
	query_stack (PrologRoot * root) {this -> root = root;}
};

class object_counter_class : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {
			drop_object_counter ();
			return true;
		}
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		AREA area;
		drop_object_counters (area);
		parameters -> setText (area);
		return true;
	}
};

class is_atom : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		return parameters -> getLeft () -> isAtom ();
	}
};

class is_integer : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		return parameters -> getLeft () -> isInteger ();
	}
};

class is_double : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		return parameters -> getLeft () -> isDouble ();
	}
};

class is_number : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		return parameters -> isInteger () || parameters -> isDouble ();
	}
};

class is_var : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		return parameters -> getLeft () -> isVar ();
	}
};

class is_head : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		return parameters -> getLeft () -> isHead ();
	}
};

class is_text : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		return parameters -> getLeft () -> isText ();
	}
};

class has_machine : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (parameters -> isAtom ()) {
			return parameters -> getAtom () -> getMachine () != NULL;
		}
		if (parameters -> isText ()) {
			PrologDirectory * dir = root -> searchDirectory (parameters -> getText ());
			if (dir == NULL) return false;
			return dir -> service_class != NULL;
		}
		return false;
	}
	has_machine (PrologRoot * root) {this -> root = root;}
};

class text_list : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		if (parameters -> getLeft () -> isText ()) {
			char * text = parameters -> getLeft () -> getText ();
			parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			parameters = parameters -> getLeft ();
			while (* text != '\0') {
				parameters -> setPair ();
				parameters -> getLeft () -> setInteger (* text++);
				parameters = parameters -> getRight ();
			}
			return true;
		}
		PrologElement * text_ptr = parameters -> getLeft ();
		AREA area;
		int area_ind = 0;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		while (parameters -> isPair ()) {
			if (! parameters -> getLeft () -> isInteger ()) return false;
			area_ind = area_cat (area, area_ind, (char) parameters -> getLeft () -> getInteger ());
			parameters = parameters -> getRight ();
		}
		text_ptr -> setText (area);
		return parameters -> isEarth ();
	}
};

class term_reader : public PrologReader {
public:
	char * text;
	virtual void message (char * text) {}
	virtual int move_z (void) {
		if (* text == '\0') return -1;
		return * text++;
	}
	void init (PrologRoot * root, char * text) {
		this -> text = text;
		setRoot (root);
	}
};

class text_term : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el;
		if (parameters -> getLeft () -> isText ()) {
			term_reader * tr = new term_reader ();
			tr -> init (root, parameters -> getLeft () -> getText ());
			parameters = parameters -> getRight ();
			parameters -> setEarth ();
			el = tr -> readElement ();
			while (el != NULL) {
				parameters -> setPair (el, root -> earth ());
				parameters = parameters -> getRight ();
				tr -> reset_var_number ();
				el = tr -> readElement ();
			}
			delete tr;
			return true;
		}
		el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		AREA area;
		int area_ind = area_cat (area, 0, "");
		while (parameters -> isPair ()) {
			if (area_ind > 0) area_ind = area_cat (area, area_ind, " ");
			area_ind = root -> getValue (parameters -> getLeft (), area, area_ind);
			parameters = parameters -> getRight ();
		}
		el -> setText (area);
		return true;
	}
	text_term (PrologRoot * root) {this -> root = root;}
};

class e32 : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		int ind = 0;
		PrologElement * el = parameters -> getLeft ();
		if (el -> isInteger ()) {
			ind = el -> getInteger ();
			parameters = parameters -> getRight (); parameters -> setPair (); parameters -> getLeft () -> setInteger (ind & 0xff);
			parameters = parameters -> getRight (); parameters -> setPair (); parameters -> getLeft () -> setInteger ((ind >> 8) & 0xff);
			parameters = parameters -> getRight (); parameters -> setPair (); parameters -> getLeft () -> setInteger ((ind >> 16) & 0xff);
			parameters = parameters -> getRight (); parameters -> setPair (); parameters -> getLeft () -> setInteger ((ind >> 24) & 0xff);
			return true;
		}
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) {el -> setInteger (ind); return true;}
		PrologElement * ex = parameters -> getLeft ();
		if (! ex -> isInteger ()) return false;
		ind = ex -> getInteger ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) {el -> setInteger (ind); return true;}
		ex = parameters -> getLeft ();
		if (! ex -> isInteger ()) return false;
		ind += ex -> getInteger () << 8;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) {el -> setInteger (ind); return true;}
		ex = parameters -> getLeft ();
		if (! ex -> isInteger ()) return false;
		ind += ex -> getInteger () << 16;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) {el -> setInteger (ind); return true;}
		ex = parameters -> getLeft ();
		if (! ex -> isInteger ()) return false;
		ind += ex -> getInteger () << 24;
		el -> setInteger (ind);
		return true;
	}
};

class sum : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e1 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e2 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		PrologElement * e3 = parameters -> isPair () ? parameters -> getLeft () : parameters;
		if (e1 -> isInteger ()) {
			if (e2 -> isInteger ()) {
				e3 -> setInteger (e1 -> getInteger () + e2 -> getInteger ());
				return true;
			}
			if (e2 -> isDouble ()) {
				e3 -> setDouble (e1 -> getInteger () + e2 -> getDouble ());
				return true;
			}
			if (e3 -> isInteger ()) {
				e2 -> setInteger (e3 -> getInteger () - e1 -> getInteger ());
				return true;
			}
			if (e3 -> isDouble ()) {
				e2 -> setDouble (e3 -> getDouble () - e1 -> getInteger ());
				return true;
			}
			return false;
		}
		if (e1 -> isDouble ()) {
			if (e2 -> isInteger ()) {
				e3 -> setDouble (e1 -> getDouble () + e2 -> getInteger ());
				return true;
			}
			if (e2 -> isDouble ()) {
				e3 -> setDouble (e1 -> getDouble () + e2 -> getDouble ());
				return true;
			}
			if (e3 -> isInteger ()) {
				e2 -> setDouble (e3 -> getInteger () - e1 -> getDouble ());
				return true;
			}
			if (e3 -> isDouble ()) {
				e2 -> setDouble (e3 -> getDouble () - e1 -> getDouble ());
				return true;
			}
			return false;
		}
		if (e2 -> isInteger ()) {
			if (e3 -> isInteger ()) {
				e1 -> setInteger (e3 -> getInteger () - e2 -> getInteger ());
				return true;
			}
			if (e3 -> isDouble ()) {
				e1 -> setDouble (e3 -> getDouble () - e2 -> getInteger ());
				return true;
			}
			return false;
		}
		if (e2 -> isDouble ()) {
			if (e3 -> isInteger ()) {
				e1 -> setDouble (e3 -> getInteger () - e2 -> getDouble ());
				return true;
			}
			if (e3 -> isDouble ()) {
				e1 -> setDouble (e3 -> getDouble () - e2 -> getDouble ());
				return true;
			}
		}
		return false;
	}
};

class times : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e1 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e2 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		PrologElement * e3 = parameters -> isPair () ? parameters -> getLeft () : parameters;
		int ind;
		double db;
		if (e1 -> isInteger ()) {
			if (e2 -> isInteger ()) {
				e3 -> setInteger (e1 -> getInteger () * e2 -> getInteger ());
				return true;
			}
			if (e2 -> isDouble ()) {
				e3 -> setDouble (e1 -> getInteger () * e2 -> getDouble ());
				return true;
			}
			ind = e1 -> getInteger ();
			if (ind == 0) return false;
			if (e3 -> isInteger ()) {
				e2 -> setInteger (e3 -> getInteger () / ind);
				return true;
			}
			if (e3 -> isDouble ()) {
				e2 -> setDouble (e3 -> getDouble () / ind);
				return true;
			}
			return false;
		}
		if (e1 -> isDouble ()) {
			if (e2 -> isInteger ()) {
				e3 -> setDouble (e1 -> getDouble () * e2 -> getInteger ());
				return true;
			}
			if (e2 -> isDouble ()) {
				e3 -> setDouble (e1 -> getDouble () * e2 -> getDouble ());
				return true;
			}
			db = e1 -> getDouble ();
			if (db == 0.0) return false;
			if (e3 -> isInteger ()) {
				e2 -> setDouble (e3 -> getInteger () / db);
				return true;
			}
			if (e3 -> isDouble ()) {
				e2 -> setDouble (e3 -> getDouble () / db);
				return true;
			}
			return false;
		}
		if (e2 -> isInteger ()) {
			ind = e2 -> getInteger ();
			if (ind == 0) return false;
			if (e3 -> isInteger ()) {
				e1 -> setInteger (e3 -> getInteger () / ind);
				return true;
			}
			if (e3 -> isDouble ()) {
				e1 -> setDouble (e3 -> getDouble () / ind);
				return true;
			}
			return false;
		}
		if (e2 -> isDouble ()) {
			db = e2 -> getDouble ();
			if (db == 0.0) return false;
			if (e3 -> isInteger ()) {
				e1 -> setDouble (e3 -> getInteger () / db);
				return true;
			}
			if (e3 -> isDouble ()) {
				e1 -> setDouble (e3 -> getDouble () / db);
				return true;
			}
		}
		return false;
	}
};

class add : public PrologNativeCode {
public:
	char * add_strings (char * area, char * text) {
		if (area == 0) return create_text (text);
		char * ret = create_text (strlen (area) + strlen (text) + 8);
		sprintf (ret, "%s%s", area, text);
		delete_text (area);
		return ret;
	}
	char * add_strings (char * area, int ind) {
		char command [128];
		sprintf (command, "%i", ind);
		return add_strings (area, command);
	}
	char * add_strings (char * area, double ind) {
		char command [256];
		sprintf (command, "%f", ind);
		return add_strings (area, command);
	}
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * result = parameters -> getLeft ();
		if (result -> isVar ()) parameters = parameters -> getRight ();
		int result_type = 0;
		int int_result = 0;
		double double_result = 0.0;
		char * area = 0;
		bool added = false;
		while (parameters -> isPair ()) {
			PrologElement * e1 = parameters -> getLeft ();
			if (e1 -> isInteger ()) {
				switch (result_type) {
				case 0: int_result += e1 -> getInteger (); break;
				case 1: double_result += (double) e1 -> getInteger (); break;
				case 2: area = add_strings (area, e1 -> getInteger ()); break;
				}
			}
			if (e1 -> isDouble ()) {
				switch (result_type) {
				case 0: double_result += e1 -> getDouble () + (double) int_result; result_type = 1; break;
				case 1: double_result += e1 -> getDouble (); break;
				case 2: area = add_strings (area, e1 -> getDouble ()); break;
				}
			}
			if (e1 -> isText () || e1 -> isAtom ()) {
				switch (result_type) {
				case 0:
					if (added) area = add_strings (area, int_result);
					area = add_strings (area, e1 -> isText () ? e1 -> getText () : e1 -> getAtom () -> name ());
					result_type = 2;
					break;
				case 1:
					area = add_strings (area, double_result);
					area = add_strings (area, e1 -> isText () ? e1 -> getText () : e1 -> getAtom () -> name ());
					result_type = 2;
					break;
				case 2: area = add_strings (area, e1 -> isText () ? e1 -> getText () : e1 -> getAtom () -> name ()); break;
				}
			}
			if (e1 -> isVar ()) {
				switch (result_type) {
				case 0: e1 -> setInteger (int_result); break;
				case 1: e1 -> setDouble (double_result); break;
				case 2: e1 -> setText (area); if (area != 0) delete_text (area); break;
				}
				return true;
			}
			parameters = parameters -> getRight ();
			added = true;
		}
		if (parameters -> isVar ()) result = parameters;
		switch (result_type) {
		case 0: result -> setInteger (int_result); break;
		case 1: result -> setDouble (double_result); break;
		case 2: result -> setText (area); if (area != 0) delete_text (area); break;
		}
		return true;
	}
};

class division : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e1 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e2 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (e2 -> isInteger ()) {
			int ind = e2 -> getInteger ();
			if (ind == 0) return false;
			if (e1 -> isInteger ()) {
				parameters -> setInteger (e1 -> getInteger () / ind);
				return true;
			}
			if (e1 -> isDouble ()) {
				parameters -> setDouble (e1 -> getDouble () / ind);
				return true;
			}
			return false;
		}
		if (e2 -> isDouble ()) {
			double db = e2 -> getDouble ();
			if (db == 0.0) return false;
			if (e1 -> isInteger ()) {
				parameters -> setDouble (e1 -> getInteger () / db);
				return true;
			}
			if (e1 -> isDouble ()) {
				parameters -> setDouble (e1 -> getDouble () / db);
				return true;
			}
		}
		return false;
	}
};

class mod : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e1 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e2 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		PrologElement * e_div = NULL;
		if (parameters -> isPair ()) {
			PrologElement * e = parameters -> getRight ();
			if (e -> isPair ()) e_div = e -> getLeft ();
			parameters = parameters -> getLeft ();
		}
		if (e2 -> isInteger ()) {
			int ind = e2 -> getInteger ();
			if (ind == 0) return false;
			if (e1 -> isInteger ()) {
				parameters -> setInteger (e1 -> getInteger () % ind);
				if (e_div != NULL) e_div -> setInteger (e1 -> getInteger () / ind);
				return true;
			}
			if (e1 -> isDouble ()) {
				parameters -> setDouble ((int) e1 -> getDouble () % ind);
				if (e_div != NULL) e_div -> setDouble ((int) e1 -> getDouble () / ind);
				return true;
			}
			return false;
		}
		if (e2 -> isDouble ()) {
			int db = (int) e2 -> getDouble ();
			if (db == 0) return false;
			if (e1 -> isInteger ()) {
				parameters -> setDouble (e1 -> getInteger () % db);
				if (e_div != NULL) e_div -> setDouble (e1 -> getInteger () / db);
				return true;
			}
			if (e1 -> isDouble ()) {
				parameters -> setDouble ((int) e1 -> getDouble () % db);
				if (e_div != NULL) e_div -> setDouble ((int) e1 -> getDouble () / db);
				return true;
			}
		}
		return false;
	}
};

class sub : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e1 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e2 = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (e1 -> isInteger ()) {
			if (e2 -> isInteger ()) {
				parameters -> setInteger (e1 -> getInteger () - e2 -> getInteger ());
				return true;
			}
			if (e2 -> isDouble ()) {
				parameters -> setDouble (e1 -> getInteger () - e2 -> getDouble ());
				return true;
			}
			return false;
		}
		if (e1 -> isDouble ()) {
			if (e2 -> isInteger ()) {
				parameters -> setDouble (e1 -> getDouble () - e2 -> getInteger ());
				return true;
			}
			if (e2 -> isDouble ()) {
				parameters -> setDouble (e1 -> getDouble () - e2 -> getDouble ());
				return true;
			}
		}
		return false;
	}
};

class mult : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * result = parameters -> getLeft ();
		if (result -> isVar ()) parameters = parameters -> getRight ();
		int result_type = 0;
		int int_result = 1;
		double double_result = 1.0;
		while (parameters -> isPair ()) {
			PrologElement * e1 = parameters -> getLeft ();
			if (e1 -> isInteger ()) {
				switch (result_type) {
				case 0: int_result *= e1 -> getInteger (); break;
				case 1: double_result *= (double) e1 -> getInteger (); break;
				}
			}
			if (e1 -> isDouble ()) {
				switch (result_type) {
				case 0: double_result *= e1 -> getDouble () * (double) int_result; result_type = 1; break;
				case 1: double_result *= e1 -> getDouble (); break;
				}
			}
			if (e1 -> isVar ()) {
				switch (result_type) {
				case 0: e1 -> setInteger (int_result); break;
				case 1: e1 -> setDouble (double_result); break;
				}
				return true;
			}
			parameters = parameters -> getRight ();
		}
		if (parameters -> isVar ()) result = parameters;
		switch (result_type) {
		case 0: result -> setInteger (int_result); break;
		case 1: result -> setDouble (double_result); break;
		}
		return true;
	}
};

class logical : public PrologNativeCode {
public:
	virtual int operation (int a, int b) {return 0;}
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * a = parameters -> getLeft ();
		if (! a -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * b = parameters -> getLeft ();
		if (! b -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		parameters -> setInteger (operation (a -> getInteger (), b -> getInteger ()));
		return true;
	}
};

class logical_and : public logical {
public:
	int operation (int a, int b) {return a & b;}
};

class logical_or : public logical {
public:
	int operation (int a, int b) {return a | b;}
};

class logical_xor : public logical {
public:
	int operation (int a, int b) {return a ^ b;}
};

class abs_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		double a;
		int ind;
		if (el -> isDouble ()) {a = el -> getDouble (); parameters -> setDouble (a >= 0.0 ? a : - a); return true;}
		if (el -> isInteger ()) {ind = el -> getInteger (); parameters -> setInteger (ind >= 0 ? ind : - ind); return true;}
		return false;
	}
};
class cos_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (el -> isDouble ()) {parameters -> setDouble (cos (el -> getDouble ())); return true;}
		if (el -> isInteger ()) {parameters -> setDouble (cos ((double) el -> getInteger () * M_PI / 180.0)); return true;}
		if (el -> isVar ()) {
			double sub = 2.0;
			if (parameters -> isDouble ()) sub = parameters -> getDouble ();
			if (parameters -> isInteger ()) sub = (double) parameters -> getInteger ();
			if (sub > 1.0 || sub < -1.0) return false;
			el -> setDouble (acos (sub));
			return true;
		}
		return false;
	}
};
class degrad_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (el -> isInteger ()) {parameters -> setDouble ((double) el -> getInteger () * M_PI / 180.0); return true;}
		if (el -> isDouble ()) {parameters -> setDouble (el -> getDouble () * M_PI / 180.0); return true;}
		if (parameters -> isDouble ()) {el -> setDouble (parameters -> getDouble () * 180.0 / M_PI); return true;}
		return false;
	}
};
class e_operation : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		parameters -> setDouble (M_E);
		return true;
	}
};
class exp_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (el -> isDouble ()) {parameters -> setDouble (exp (el -> getDouble ())); return true;}
		if (el -> isInteger ()) {parameters -> setInteger ((int) (exp ((double) el -> getInteger ()))); return true;}
		if (parameters -> isDouble ()) {double d = parameters -> getDouble (); if (d <= 0.0) return false; el -> setDouble (log (d)); return true;}
		if (parameters -> isInteger ()) {int d = parameters -> getInteger (); if (d <= 0) return false; el -> setInteger ((int) log ((double) d)); return true;}
		return false;
	}
};
class log2_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ex = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (ex -> isDouble ()) {double x = ex -> getDouble (); if (x <= 0.0) return false; parameters -> setDouble (log (x) / 0.69314718055994530941723212145818); return true;}
		if (ex -> isInteger ()) {int x = ex -> getInteger (); if (x <= 0) return false; parameters -> setInteger ((int) (log ((double) x) / 0.69314718055994530941723212145818)); return true;}
		if (parameters -> isDouble ()) {ex -> setDouble (pow (2.0, parameters -> getDouble ())); return true;}
		if (parameters -> isInteger ()) {ex -> setInteger ((int) pow (2.0, (double) parameters -> getInteger ())); return true;}
		return false;
	}
};
class log10_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ex = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (ex -> isDouble ()) {double x = ex -> getDouble (); if (x <= 0.0) return false; parameters -> setDouble (log10 (x)); return true;}
		if (ex -> isInteger ()) {int x = ex -> getInteger (); if (x <= 0) return false; parameters -> setInteger ((int) log10 ((double) x)); return true;}
		if (parameters -> isDouble ()) {ex -> setDouble (pow (10.0, parameters -> getDouble ())); return true;}
		if (parameters -> isInteger ()) {ex -> setInteger ((int) pow (10.0, (double) parameters -> getInteger ())); return true;}
		return false;
	}
};
class ln_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ex = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (ex -> isDouble ()) {double x = ex -> getDouble (); if (x <= 0.0) return false; parameters -> setDouble (log (x)); return true;}
		if (ex -> isInteger ()) {int x = ex -> getInteger (); if (x <= 0) return false; parameters -> setInteger ((int) log ((double) x)); return true;}
		if (parameters -> isDouble ()) {ex -> setDouble (exp (parameters -> getDouble ())); return true;}
		if (parameters -> isInteger ()) {ex -> setInteger ((int) exp ((double) parameters -> getInteger ())); return true;}
		return false;
	}
};
class log_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ea = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * ex = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		PrologElement * en = parameters -> isPair () ? parameters -> getLeft () : parameters;
		bool integer = true;
		double a, x, n;
		if (ea -> isDouble ()) {a = ea -> getDouble (); integer = false;}
		else {if (! ea -> isInteger ()) return false; a = (double) ea -> getInteger ();}
		if (ex -> isDouble ()) {x = ex -> getDouble (); integer = false;}
		else {if (! ex -> isInteger ()) return false; x = (double) ex -> getInteger ();}
		if (a <= 0.0) return false;
		if (a == 1.0) return false;
		if (x <= 0.0) return false;
		n = log (x) / log (a);
		if (integer) en -> setInteger ((int) n);
		else en -> setDouble (n);
		return true;
	}
};
class pi_operation : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		parameters -> setDouble (M_PI);
		return true;
	}
};
class pow_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ea = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * en = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		PrologElement * ex = parameters -> isPair () ? parameters -> getLeft () : parameters;
		if (ea -> isDouble ()) {
			double a = ea -> getDouble ();
			if (en -> isDouble ()) {ex -> setDouble (pow (a, en -> getDouble ())); return true;}
			if (en -> isInteger ()) {ex -> setDouble (pow (a, (double) en -> getInteger ())); return true;}
			if (a <= 0.0 || a == 1.0) return false;
			if (ex -> isDouble ()) {double x = ex -> getDouble (); if (x <= 0.0) return false; en -> setDouble (log (x) / log (a)); return true;}
			if (ex -> isInteger ()) {int x = ex -> getInteger (); if (x <= 0) return false; en -> setDouble (log ((double) x) / log (a)); return true;}
		}
		if (ea -> isInteger ()) {
			int a = ea -> getInteger ();
			if (en -> isDouble ()) {ex -> setDouble (pow ((double) a, en -> getDouble ())); return true;}
			if (en -> isInteger ()) {ex -> setInteger ((int) pow ((double) a, (double) en -> getInteger ())); return true;}
			if (a <= 0 || a == 1) return false;
			if (ex -> isDouble ()) {double x = ex -> getDouble (); if (x <= 0.0) return false; en -> setDouble (log (x) / log ((double) a)); return true;}
			if (ex -> isInteger ()) {int x = ex -> getInteger (); if (x <= 0) return false; en -> setInteger ((int) (log ((double) x) / log ((double) a))); return true;}
		}
		if (en -> isDouble ()) {
			double n = en -> getDouble ();
			if (n == 0.0) return false;
			n = 1.0 / n;
			if (ex -> isDouble ()) {ea -> setDouble (pow (ex -> getDouble (), n)); return true;}
			if (ex -> isInteger ()) {ea -> setDouble (pow ((double) ex -> getInteger (), n)); return true;}
		}
		if (en -> isInteger ()) {
			double n = (double) en -> getInteger ();
			if (n == 0.0) return false;
			n = 1.0 / n;
			if (ex -> isDouble ()) {ea -> setDouble (pow (ex -> getDouble (), n)); return true;}
			if (ex -> isInteger ()) {ea -> setInteger ((int) pow ((double) ex -> getInteger (), n)); return true;}
		}
		return false;
	}
};
class sin_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (el -> isDouble ()) {parameters -> setDouble (sin (el -> getDouble ())); return true;}
		if (el -> isInteger ()) {parameters -> setDouble (sin ((double) el -> getInteger () * M_PI / 180.0)); return true;}
		if (el -> isVar ()) {
			double sub = 2.0;
			if (parameters -> isDouble ()) sub = parameters -> getDouble ();
			if (parameters -> isInteger ()) sub = (double) parameters -> getInteger ();
			if (sub > 1.0 || sub < -1.0) return false;
			el -> setDouble (asin (sub));
			return true;
		}
		return false;
	}
};
class tan_operation : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (el -> isDouble ()) {parameters -> setDouble (tan (el -> getDouble ())); return true;}
		if (el -> isInteger ()) {parameters -> setDouble (tan ((double) el -> getInteger () * M_PI / 180.0)); return true;}
		if (parameters -> isDouble ()) {el -> setDouble (atan (parameters -> getDouble ())); return true;}
		if (parameters -> isInteger ()) {el -> setDouble (atan ((double) parameters -> getInteger ())); return true;}
		return false;
	}
};
class trunc_operation : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * left = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (left -> isDouble ()) {parameters -> setInteger ((int) left -> getDouble ()); return true;}
		if (left -> isInteger ()) {parameters -> setDouble ((double) left -> getInteger ()); return true;}
		if (left -> isVar ()) {
			if (parameters -> isInteger ()) {left -> setDouble ((double) parameters -> getInteger ()); return true;}
			if (parameters -> isDouble ()) {left -> setDouble ((double) ((int) parameters -> getDouble ())); return true;}
			return false;
		}
		return false;
	}
};

class timestamp : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * stamp = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * year = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * month = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * day = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * year_day = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * week_day = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * hour = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * minute = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) parameters -> setPair ();
		PrologElement * second = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (year -> isVar () || year -> isEarth ()) {
			time_t time_stamp = 0;
			if (stamp -> isInteger ()) time_stamp = (time_t) stamp -> getInteger ();
			else {time_stamp = time (NULL); stamp -> setInteger ((int) time_stamp);}
			struct tm * ptm;
			ptm = localtime (& time_stamp);
			year -> setInteger (1900 + ptm -> tm_year);
			month -> setInteger (ptm -> tm_mon + 1);
			day -> setInteger (ptm -> tm_mday);
			year_day -> setInteger (ptm -> tm_yday);
			week_day -> setInteger (ptm -> tm_wday);
			hour -> setInteger (ptm -> tm_hour);
			minute -> setInteger (ptm -> tm_min);
			second -> setInteger (ptm -> tm_sec);
			return true;
		}
		time_t time_stamp = time (NULL);
		struct tm * tmp;
		tmp = localtime (& time_stamp);
		if (year -> isInteger ()) tmp -> tm_year = year -> getInteger () - 1900;
		if (month -> isInteger ()) tmp -> tm_mon = month -> getInteger () - 1;
		if (day -> isInteger ()) tmp -> tm_mday = day -> getInteger ();
		if (year_day -> isInteger ()) tmp -> tm_yday = year_day -> getInteger ();
		if (week_day -> isInteger ()) tmp -> tm_wday = week_day -> getInteger ();
		if (hour -> isInteger ()) tmp -> tm_hour = hour -> getInteger ();
		if (minute -> isInteger ()) tmp -> tm_min = minute -> getInteger ();
		if (second -> isInteger ()) tmp -> tm_sec = second -> getInteger ();
		time_stamp = mktime (tmp);
		if (! stamp -> isInteger ()) stamp -> setInteger ((int) time_stamp);
		if (! year -> isInteger ()) year -> setInteger (1900 + tmp -> tm_year);
		if (! month -> isInteger ()) month -> setInteger (tmp -> tm_mon + 1);
		if (! day -> isInteger ()) day -> setInteger (tmp -> tm_mday);
		if (! year_day -> isInteger ()) year_day -> setInteger (tmp -> tm_yday);
		if (! week_day -> isInteger ()) week_day -> setInteger (tmp -> tm_wday);
		if (! hour -> isInteger ()) hour -> setInteger (tmp -> tm_hour);
		if (! minute -> isInteger ()) minute -> setInteger (tmp -> tm_min);
		if (! second -> isInteger ()) second -> setInteger (tmp -> tm_sec);
		return true;
	}
};

class less : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * a = parameters -> getLeft ();
		PrologElement * b;
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			b = parameters -> getLeft ();
			if (a -> isInteger ()) {
				if (b -> isInteger ()) {if (a -> getInteger () >= b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if ((double) a -> getInteger () >= b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isDouble ()) {
				if (b -> isInteger ()) {if (a -> getDouble () >= (double) b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if (a -> getDouble () >= b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isText ()) {
				if (b -> isText ()) {if (strcmp (a -> getText (), b -> getText ()) >= 0) return false;}
				else if (b -> isAtom ()) {if (strcmp (a -> getText (), b -> getAtom () -> name ()) >= 0) return false;}
				else return false;
			}
			else if (a -> isAtom ()) {
				if (b -> isAtom ()) {if (strcmp (a -> getAtom () -> name (), b -> getAtom () -> name ()) >= 0) return false;}
				else if (b -> isText ()) {if (strcmp (a -> getAtom () -> name (), b -> getText ()) >= 0) return false;}
				else return false;
			}
			else return false;
			a = b;
			parameters = parameters -> getRight ();
		}
		return true;
	}
};

class less_eq : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * a = parameters -> getLeft ();
		PrologElement * b;
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			b = parameters -> getLeft ();
			if (a -> isInteger ()) {
				if (b -> isInteger ()) {if (a -> getInteger () > b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if ((double) a -> getInteger () > b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isDouble ()) {
				if (b -> isInteger ()) {if (a -> getDouble () > (double) b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if (a -> getDouble () > b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isText ()) {
				if (b -> isText ()) {if (strcmp (a -> getText (), b -> getText ()) > 0) return false;}
				else if (b -> isAtom ()) {if (strcmp (a -> getText (), b -> getAtom () -> name ()) > 0) return false;}
				else return false;
			}
			else if (a -> isAtom ()) {
				if (b -> isAtom ()) {if (strcmp (a -> getAtom () -> name (), b -> getAtom () -> name ()) > 0) return false;}
				else if (b -> isText ()) {if (strcmp (a -> getAtom () -> name (), b -> getText ()) > 0) return false;}
				else return false;
			}
			else return false;
			a = b;
			parameters = parameters -> getRight ();
		}
		return true;
	}
};

class greater : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * a = parameters -> getLeft ();
		PrologElement * b;
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			b = parameters -> getLeft ();
			if (a -> isInteger ()) {
				if (b -> isInteger ()) {if (a -> getInteger () <= b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if ((double) a -> getInteger () <= b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isDouble ()) {
				if (b -> isInteger ()) {if (a -> getDouble () <= (double) b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if (a -> getDouble () <= b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isText ()) {
				if (b -> isText ()) {if (strcmp (a -> getText (), b -> getText ()) <= 0) return false;}
				else if (b -> isAtom ()) {if (strcmp (a -> getText (), b -> getAtom () -> name ()) <= 0) return false;}
				else return false;
			}
			else if (a -> isAtom ()) {
				if (b -> isAtom ()) {if (strcmp (a -> getAtom () -> name (), b -> getAtom () -> name ()) <= 0) return false;}
				else if (b -> isText ()) {if (strcmp (a -> getAtom () -> name (), b -> getText ()) <= 0) return false;}
				else return false;
			}
			else return false;
			a = b;
			parameters = parameters -> getRight ();
		}
		return true;
	}
};

class greater_eq : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * a = parameters -> getLeft ();
		PrologElement * b;
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			b = parameters -> getLeft ();
			if (a -> isInteger ()) {
				if (b -> isInteger ()) {if (a -> getInteger () < b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if ((double) a -> getInteger () < b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isDouble ()) {
				if (b -> isInteger ()) {if (a -> getDouble () < (double) b -> getInteger ()) return false;}
				else if (b -> isDouble ()) {if (a -> getDouble () < b -> getDouble ()) return false;}
				else return false;
			}
			else if (a -> isText ()) {
				if (b -> isText ()) {if (strcmp (a -> getText (), b -> getText ()) < 0) return false;}
				else if (b -> isAtom ()) {if (strcmp (a -> getText (), b -> getAtom () -> name ()) < 0) return false;}
				else return false;
			}
			else if (a -> isAtom ()) {
				if (b -> isAtom ()) {if (strcmp (a -> getAtom () -> name (), b -> getAtom () -> name ()) < 0) return false;}
				else if (b -> isText ()) {if (strcmp (a -> getAtom () -> name (), b -> getText ()) < 0) return false;}
				else return false;
			}
			else return false;
			a = b;
			parameters = parameters -> getRight ();
		}
		return true;
	}
};

class max_class : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * res = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * maximum = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			PrologElement * e = parameters -> getLeft ();
			if (e -> isInteger ()) {
				if (maximum -> isInteger ()) {if (e -> getInteger () > maximum -> getInteger ()) maximum = e;}
				else if (maximum -> isDouble ()) {if ((double) e -> getInteger () > maximum -> getDouble ()) maximum = e;}
				else return false;
			} else if (e -> isDouble ()) {
				if (maximum -> isDouble ()) {if (e -> getDouble () > maximum -> getDouble ()) maximum = e;}
				else if (maximum -> isInteger ()) {if (e -> getDouble () > (double) maximum -> getInteger ()) maximum = e;}
				else return false;
			} else if (e -> isText ()) {
				if (maximum -> isText ()) {if (strcmp (e -> getText (), maximum -> getText ()) > 0) maximum = e;}
				else if (maximum -> isAtom ()) {if (strcmp (e -> getText (), maximum -> getAtom () -> name ()) > 0) maximum = e;}
				else return false;
			} else if (e -> isAtom ()) {
				if (maximum -> isAtom ()) {if (strcmp (e -> getAtom () -> name (), maximum -> getAtom () -> name ()) > 0) maximum = e;}
				else if (maximum -> isText ()) {if (strcmp (e -> getAtom () -> name (), maximum -> getText ()) > 0) maximum = e;}
				else return false;
			} else return false;
			parameters = parameters -> getRight ();
		}
		if (maximum -> isInteger ()) {res -> setInteger (maximum -> getInteger ()); return true;}
		if (maximum -> isDouble ()) {res -> setDouble (maximum -> getDouble ()); return true;}
		if (maximum -> isText ()) {res -> setText (maximum -> getText ()); return true;}
		if (maximum -> isAtom ()) {res -> setAtom (maximum -> getAtom ()); return true;}
		return false;
	}
};

class min_class : public PrologNativeCode {
public:
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * res = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * minimum = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			PrologElement * e = parameters -> getLeft ();
			if (e -> isInteger ()) {
				if (minimum -> isInteger ()) {if (e -> getInteger () < minimum -> getInteger ()) minimum = e;}
				else if (minimum -> isDouble ()) {if ((double) e -> getInteger () < minimum -> getDouble ()) minimum = e;}
				else return false;
			} else if (e -> isDouble ()) {
				if (minimum -> isDouble ()) {if (e -> getDouble () < minimum -> getDouble ()) minimum = e;}
				else if (minimum -> isInteger ()) {if (e -> getDouble () < (double) minimum -> getInteger ()) minimum = e;}
				else return false;
			} else if (e -> isText ()) {
				if (minimum -> isText ()) {if (strcmp (e -> getText (), minimum -> getText ()) < 0) minimum = e;}
				else if (minimum -> isAtom ()) {if (strcmp (e -> getText (), minimum -> getAtom () -> name ()) < 0) minimum = e;}
				else return false;
			} else if (e -> isAtom ()) {
				if (minimum -> isAtom ()) {if (strcmp (e -> getAtom () -> name (), minimum -> getAtom () -> name ()) < 0) minimum = e;}
				else if (minimum -> isText ()) {if (strcmp (e -> getAtom () -> name (), minimum -> getText ()) < 0) minimum = e;}
				else return false;
			} else return false;
			parameters = parameters -> getRight ();
		}
		if (minimum -> isInteger ()) {res -> setInteger (minimum -> getInteger ()); return true;}
		if (minimum -> isDouble ()) {res -> setDouble (minimum -> getDouble ()); return true;}
		if (minimum -> isText ()) {res -> setText (minimum -> getText ()); return true;}
		if (minimum -> isAtom ()) {res -> setAtom (minimum -> getAtom ()); return true;}
		return false;
	}
};

class file_write : public PrologNativeCode {
public:
	FILE * fw;
	PrologRoot * root;
	PrologAtom * atom;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		AREA area;
		if (fw == NULL) return false;
		PrologElement * el;
		if (parameters -> isEarth ()) {
			fclose (fw);
			fw = 0;
			atom -> setMachine (0);
			delete this;
			return true;
		}
		while (parameters -> isPair ()) {
			el = parameters -> getLeft ();
			if (el -> isText ()) fprintf (fw, "%s", el -> getText ());
			if (el -> isInteger ()) fputc (el -> getInteger (), fw);
			while (el -> isPair ()) {
				root -> getValue (el -> getLeft (), area, 0);
				fprintf (fw, "%s", area);
				el = el -> getRight ();
			}
			parameters = parameters -> getRight ();
		}
		return true;
	}
	file_write (PrologAtom * atom, PrologRoot * root, char * file_name) {
		this -> atom = atom;
		this -> root = root;
		fw = fopen (file_name, "wb");
	}
	~ file_write (void) {if (fw != NULL) fclose (fw);}
};

class file_writer : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ea = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (ea -> isVar ()) {
			atom = new PrologAtom ();
			ea -> setAtom (atom);
		}
		else {
			if (! ea -> isAtom ()) return false;
			atom = ea -> getAtom ();
		}
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * et = parameters -> getLeft ();
		if (! et -> isText ()) return false;
		file_write * fw = new file_write (atom, root, et -> getText ());
		if (atom -> setMachine (fw)) return true;
		delete fw;
		return false;
	}
	file_writer (PrologRoot * root) {this -> root = root;}
};

class symbol_reader : public PrologReader {
public:
	FILE * fi;
	void message (char * text) {root -> print (text); root -> print (root -> new_line_caption);}
	void message_v (char * text, char * variable) {root -> message (text, variable);}
	int move_z (void) {return fgetc (fi);}
	void init (PrologRoot * root, FILE * fi) {
		this -> fi = fi;
		setRoot (root);
	}
};

class file_read : public PrologNativeCode {
public:
	FILE * fi;
	symbol_reader sr;
	PrologAtom * atom;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (fi == NULL) {
			if (! parameters -> isEarth ()) return false;
			atom -> setMachine (0);
			delete this;
			return true;
		}
		if (parameters -> isEarth ()) {
			fclose (fi);
			fi = NULL;
			atom -> setMachine (0);
			delete this;
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * el = parameters -> getLeft ();
		if (el -> isText ()) {
			parameters = parameters -> getRight ();
			if (! parameters -> isPair ()) return false;
			char * char_set = el -> getText ();
			AREA area;
			if (sr . getString (area, 0, char_set) < 0) return false;
			parameters -> getLeft () -> setText (area);
			return true;
		}
		el = sr . readElement ();
		if (el == NULL) return false;
		parameters -> setLeft (el);
		return true;
	}
	file_read (PrologAtom * atom, PrologRoot * root, char * file_name) {
		this -> atom = atom;
		if (
			(file_name [1] == ':' && file_name [2] == '\\')
			|| (file_name [0] == '~' && file_name [1] == '/')
			|| file_name [0] == '/'
			) fi = fopen (file_name, "rb");
		else {
			fi = fopen (file_name, "rb");
		}
		sr . init (root, fi);
	}
	~ file_read (void) {if (fi != NULL) fclose (fi);}
};

class file_reader : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ea = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (ea -> isVar ()) {
			atom = new PrologAtom ();
			ea -> setAtom (atom);
		} else {
			if (! ea -> isAtom ()) return false;
			atom = ea -> getAtom ();
		}
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * et = parameters -> getLeft ();
		if (! et -> isText ()) return false;
		file_read * fr = new file_read (atom, root, et -> getText ());
		if (fr -> fi == NULL) {delete fr; return false;}
		if (atom -> setMachine (fr)) return true;
		delete fr;
		return false;
	}
	file_reader (PrologRoot * root) {this -> root = root;}
};

class module_loader : public PrologNativeCode {
public:
	PrologRoot * root;
	bool echo;
	bool reload;
	bool code (PrologElement * parameters, PrologResolution * resulotion) {
		while (parameters -> isPair ()) {
			PrologElement * module_name = parameters -> getLeft ();
			if (! module_name -> isText ()) return false;
			PrologLoader loader (root);
			loader . echo = echo;
			loader . reload = reload;
			if (strstr (module_name -> getText (), ".prc") == NULL) {
				PROLOG_STRING command;
				prolog_string_copy (command, module_name -> getText ());
				prolog_string_cat (command, ".prc");
				if (! loader . load_without_main (command)) return false;
			} else {if (! loader . load_without_main (module_name -> getText ())) return false;}
			parameters = parameters -> getRight ();
		}
		return true;
	}
};

class import_loader : public module_loader {
public:
	import_loader (PrologRoot * root) {
		this -> root = root;
		this -> echo = false;
		this -> reload = false;
	}
};

class load_loader : public module_loader {
public:
	load_loader (PrologRoot * root) {
		this -> root = root;
		this -> echo = false;
		this -> reload = true;
	}
};

class consult_loader : public module_loader {
public:
	consult_loader (PrologRoot * root) {
		this -> root = root;
		this -> echo = true;
		this -> reload = true;
	}
};

class remove_module : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		while (parameters -> isPair ()) {
			PrologElement * drop = parameters -> getLeft ();
			if (! drop -> isText ()) return false;
			if (! root -> drop (drop -> getText ())) return false;
			parameters = parameters -> getRight ();
		}
		return true;
	}
	remove_module (PrologRoot * root) {this -> root = root;}
};

class create_module : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {root -> close (); return true;}
		if (! parameters -> isPair ()) return false;
		PrologElement * em = parameters -> getLeft ();
		if (! em -> isText ()) return false;
		parameters = parameters -> getRight ();
		if (parameters -> isEarth ()) {root -> createDirectory (em -> getText ()); return true;}
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		PrologServiceClass * service = root -> loadServiceClass (parameters -> getText ());
		if (service == NULL) return false;
		root -> createDirectory (em -> getText (), service);
		return true;
	}
	create_module (PrologRoot * root) {this -> root = root;}
};

class set_machine : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * ea = parameters -> getLeft ();
		if (! ea -> isAtom ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * et = parameters -> getLeft ();
		if (! et -> isText ()) return false;
		parameters = parameters -> getRight ();
		PrologServiceClass * service;
		if (parameters -> isEarth ()) {
			service = root -> getServiceClass ();
			if (service == NULL) return false;
			PrologNativeCode * native = service -> getNativeCode (et -> getText ());
			if (native == NULL) return false;
			bool ret = ea -> getAtom () -> setMachine (native);
			if (! ret) delete native;
			return ret;
		}
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		service = root -> getServiceClass (et -> getText ());
		if (service == NULL) return false;
		PrologNativeCode * native = service -> getNativeCode (parameters -> getText ());
		if (native == NULL ) return false;
		bool ret = ea -> getAtom () -> setMachine (native);
		if (! ret) delete native;
		return ret;
	}
	set_machine (PrologRoot * root) {this -> root = root;}
};

class add_search_directory : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		while (parameters -> isPair ()) {
			PrologElement * dir = parameters -> getLeft ();
			if (! dir -> isText ()) return false;
			root -> addSearchDirectory (dir -> getText ());
			parameters = parameters -> getRight ();
		}
		return true;
	}
	add_search_directory (PrologRoot * root) {this -> root = root;}
};

class search_directories : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {
			printf ("SEARCH DIRECTORIES:\n");
			PrologString * search_directory = root -> search_directories;
			while (search_directory != NULL) {
				printf ("	[%s]\n", search_directory -> text);
				search_directory = search_directory -> next;
			}
			return true;
		}
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (parameters -> isEarth ()) {root -> deleteSearchDirectories (); return true;}
		if (parameters -> isVar ()) {
			PrologString * search_directory = root -> search_directories;
			if (search_directory == NULL) {parameters -> setEarth (); return true;}
			while (search_directory != NULL) {
				parameters -> setPair ();
				parameters -> getLeft () -> setText (search_directory -> text);
				parameters = parameters -> getRight ();
				search_directory = search_directory -> next;
			}
			return true;
		}
		if (parameters -> isPair ()) {
			PrologString * search_directory = NULL;
			root -> deleteSearchDirectories ();
			while (parameters -> isPair ()) {
				if (parameters -> getLeft () -> isText ()) {
					PrologString * dir = new PrologString (parameters -> getLeft () -> getText (), NULL);
					if (root -> search_directories == NULL) root -> search_directories = search_directory = dir;
					else {search_directory -> next = dir; search_directory = search_directory -> next;}
				}
				parameters = parameters -> getRight ();
			}
			return true;
		}
		return false;
	}
	search_directories (PrologRoot * root) {this -> root = root;}
};

class cd : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {return root -> change_directory ("..");}
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (parameters -> isVar ()) {parameters -> setText (root -> getCWD ()); return true;}
		if (parameters -> isText ()) return root -> change_directory (parameters -> getText ());
		return false;
	}
	cd (PrologRoot * root) {this -> root = root;}
};

class DIR : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * left = parameters -> getLeft ();
		if (! left -> isText ()) return false;
		PrologElement * el = root -> dir (left -> getText ());
		if (el == 0) return false;
		parameters -> setRight (el);
		return true;
	}
	DIR (PrologRoot * root) {this -> root = root;}
};

class ARGS : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (root -> args == NULL) {parameters -> setEarth (); return true;}
		PrologString * sp = root -> args;
		while (sp != NULL) {parameters -> setPair (); parameters -> getLeft () -> setText (sp -> text); parameters = parameters -> getRight (); sp = sp -> next;}
		return true;
	}
	ARGS (PrologRoot * root) {this -> root = root;}
};

class edit : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		return root -> edit (parameters -> getText ());
	}
	edit (PrologRoot * root) {this -> root = root;}
};

class execute : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		return root -> execute (parameters -> getText ());
	}
	execute (PrologRoot * root) {this -> root = root;}
};

class make_directory : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		while (parameters -> isPair ()) {
			PrologElement * dir = parameters -> getLeft ();
			if (! dir -> isText ()) return false;
			if (! root -> make_directory (dir -> getText ())) return false;
			parameters = parameters -> getRight ();
		}
		return true;
	}
	make_directory (PrologRoot * root) {this -> root = root;}
};

class erase_file : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		while (parameters -> isPair ()) {
			PrologElement * eraser = parameters -> getLeft ();
			if (! eraser -> isText ()) return false;
			if (! root -> erase_file (eraser -> getText ())) return false;
			parameters = parameters -> getRight ();
		}
		return true;
	}
	erase_file (PrologRoot * root) {this -> root = root;}
};

class erase_directory : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		while (parameters -> isPair ()) {
			PrologElement * eraser = parameters -> getLeft ();
			if (! eraser -> isText ()) return false;
			if (! root -> erase_directory (eraser -> getText ())) return false;
			parameters = parameters -> getRight ();
		}
		return true;
	}
	erase_directory (PrologRoot * root) {this -> root = root;}
};

class move_file : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * from = parameters -> getLeft ();
		if (! from -> isText ()) return false;
		parameters = parameters -> getRight ();

		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		return root -> move_file (from -> getText (), parameters -> getText ());
	}
	move_file (PrologRoot * root) {this -> root = root;}
};

class copy_file : public PrologNativeCode {
private:
	PrologRoot * root;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * from = parameters -> getLeft ();
		if (! from -> isText ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isText ()) return false;
		return root -> copy_file (from -> getText (), parameters -> getText ());
	}
	copy_file (PrologRoot * root) {this -> root = root;}
};

//////////
// META //
//////////

class constant : public PrologNativeCode {
private:
	PrologElement * container;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		parameters -> duplicate (container);
		return true;
	}
	constant (PrologElement * term) {container = term -> duplicate ();}
	~ constant (void) {delete container;}
};

class CONSTANT : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (e -> isVar ()) {
			atom = new PrologAtom ();
			e -> setAtom (atom);
		} else {
			if (! e -> isAtom ()) return false;
			atom = e -> getAtom ();
		}
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		constant * c = new constant (parameters -> getLeft ());
		if (atom -> setMachine (c)) return true;
		delete c;
		return false;
	}
};

class variable : public PrologNativeCode {
private:
	PrologAtom * atom;
	PrologElement * container;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) {
			delete container;
			container = parameters -> getLeft () -> duplicate ();
			return true;
		}
		if (parameters -> isVar ()) {
			parameters -> duplicate (container);
			return true;
		}
		if (parameters -> isEarth ()) {
			atom -> setMachine (0);
			delete this;
			return true;
		}
		return false;
	}
	variable (PrologAtom * atom) {
		this -> atom = atom;
		container = new PrologElement ();
	}
	variable (PrologAtom * atom, PrologElement * element) {
		this -> atom = atom;
		container = element -> duplicate ();
	}
	~ variable (void) {delete container;}
};

class VARIABLE : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * initial_value = parameters -> getRight ();
		parameters = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (parameters -> isVar ()) {
			atom = new PrologAtom ();
			parameters -> setAtom (atom);
		} else {
			if (! parameters -> isAtom ()) return false;
			atom = parameters -> getAtom ();
		}
		variable * v;
		if (initial_value -> isPair ()) v = new variable (atom, initial_value -> getLeft ());
		else v = new variable (atom);
		if (atom -> setMachine (v)) return true;
		delete v;
		return false;
	}
};

class accumulator : public PrologNativeCode {
private:
	PrologAtom * atom;
	PrologElement * container;
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) {
			resolution -> reset ();
			PrologElement * e = new PrologElement ();
			e -> setPair (resolution -> match_product (parameters -> getLeft (), true), resolution -> match_product (container, false));
			delete container;
			container = e;
			return true;
		}
		if (parameters -> isVar ()) {
			parameters -> duplicate (container);
			return true;
		}
		if (parameters -> isEarth ()) {
			atom -> setMachine (0);
			delete this;
			return true;
		}
		return false;
	}
	accumulator (PrologAtom * atom) {
		this -> atom = atom;
		container = new PrologElement ();
	}
	~ accumulator (void) {delete container;}
};

class ACCUMULATOR : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (parameters -> isVar ()) {
			atom = new PrologAtom ();
			parameters -> setAtom (atom);
		} else {
			if (! parameters -> isAtom ()) return false;
			atom = parameters -> getAtom ();
		}
		accumulator * accu = new accumulator (atom);
		if (atom -> setMachine (accu)) return true;
		delete accu;
		return false;
	}
};

typedef void * void_pointer;
class array_dimension {
public:
	void_pointer * content;
	int size;
	bool locked;
	bool elements;
	void insert_dimension (int size) {
		int ind;
		if (locked)
			for (ind = 0; ind < this -> size; ind++)
				((array_dimension *) (content [ind])) -> insert_dimension (size);
		else
			for (ind = 0; ind < this -> size; ind++)
				content [ind] = (void *) new array_dimension (size);
		locked = true;
	}
	void insert_elements (void) {
		int ind;
		if (elements) return;
		if (locked)
			for (ind = 0; ind < size; ind++)
				((array_dimension *) (content [ind])) -> insert_elements ();
		else {
			for (ind = 0; ind < size; ind++)
				content [ind] = (void *) new PrologElement ();
			elements = true;
		}
		locked = true;
	}
	array_dimension * get_dimension (int location) {
		if (location < 0 || location >= size || elements) return NULL;
		return (array_dimension *) (content [location]);
	}
	PrologElement * * get_element (int location) {
		if (location < 0 || location >= size || ! elements) return NULL;
		return (PrologElement * *) (& content [location]);
	}
	array_dimension (int size) {
		content = new void_pointer [size];
		this -> size = size;
		locked = false;
		elements = false;
	}
	~ array_dimension (void) {
		int ind;
		if (elements)
			for (ind = 0; ind < size; ind++)
				delete (PrologElement *) (content [ind]);
		else
			for (ind = 0; ind < size; ind++)
				delete (array_dimension *) (content [ind]);
		delete [] content;
	}
};

class array : public PrologNativeCode {
private:
	PrologAtom * atom;
	array_dimension * dimension;
	int dimensions;
public:
	void insert_dimension (int size) {
		if (dimension == NULL) dimension = new array_dimension (size);
		else dimension -> insert_dimension (size);
		dimensions++;
	}
	void insert_elements (void) {
		if (dimension == NULL) return;
		dimension -> insert_elements ();
	}
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {
			atom -> setMachine (0);
			delete this;
			return true;
		}
		int ind = dimensions;
		array_dimension * current_dimension = dimension;
		PrologElement * left;
		while (ind > 1) {
			if (! parameters -> isPair ()) return false;
			left = parameters -> getLeft ();
			if (! left -> isInteger ()) return false;
			parameters = parameters -> getRight ();
			current_dimension = current_dimension -> get_dimension (left -> getInteger ());
			if (current_dimension == NULL) return false;
			ind--;
		}
		if (! parameters -> isPair ()) return false;
		left = parameters -> getLeft ();
		if (! left -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		PrologElement * * container = current_dimension -> get_element (left -> getInteger ());
		if (container == NULL) return false;
		if (parameters -> isPair ()) {
			delete * container;
			* container = parameters -> getLeft () -> duplicate ();
			return true;
		}
		if (parameters -> isVar ()) {
			parameters -> duplicate (* container);
			return true;
		}
		return false;
	}
	array (PrologAtom * atom) {
		this -> atom = atom;
		dimension = NULL;
		dimensions = 0;
	}
	~ array (void) {if (dimension) delete dimension;}
};

class ARRAY : public PrologNativeCode {
public:
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * left = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (left -> isVar ()) {
			atom = new PrologAtom ();
			left -> setAtom (atom);
		} else {
			if (! left -> isAtom ()) return false;
			atom = left -> getAtom ();
		}
		array * a = new array (atom);
		parameters = parameters -> getRight ();
		while (parameters -> isPair ()) {
			left = parameters -> getLeft ();
			if (! left -> isInteger ()) {
				delete a;
				return false;
			}
			a -> insert_dimension (left -> getInteger ());
			parameters = parameters -> getRight ();
		}
		a -> insert_elements ();
		if (atom -> setMachine (a)) return true;
		delete a;
		return false;
	}
};

/////////////////////
// NOISE GENERATOR //
/////////////////////

int PrologNoise :: get (void) {v = (v * a + c) & m; return v;}
int PrologNoise :: get (int min, int max) {
	float out = (float) get () * (float) (max - min);
	out /= (float) range;
	return min + (int) out;
}
void PrologNoise :: resolution (int bits) {range = 1 << bits; m = range - 1;}
void PrologNoise :: control (int v) {this -> v = v;}
void PrologNoise :: control (int a, int c) {this -> a = a; this -> c = c;}
PrologNoise :: PrologNoise (void) TRACKING (4) {resolution (24); control (0); control (0x5599d1, 1);}

class rnd : public PrologNativeCode {
public:
	PrologNoise * n;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isVar ()) {parameters -> setInteger (n -> get ()); return true;}
		if (! parameters -> isPair ()) return false;
		PrologElement * a = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isEarth ()) {a -> setInteger (n -> get ()); return true;}
		if (! parameters -> isPair ()) return false;
		PrologElement * b = parameters -> getLeft ();
		PrologElement * c = parameters -> getRight ();
		if (c -> isPair ()) c = c -> getLeft ();
		if (a -> isInteger ()) {
			if (b -> isInteger ()) {
				c -> setInteger (n -> get (a -> getInteger (), b -> getInteger ()));
				return true;
			} else if (c -> isInteger ()) {
				b -> setInteger (n -> get (a -> getInteger (), c -> getInteger ()));
				return true;
			} else return false;
		}
		if (b -> isInteger ()) {
			if (c -> isInteger ()) {
				a -> setInteger (n -> get (b -> getInteger (), c -> getInteger ()));
				return true;
			}
		}
		return false;
	}
	rnd (PrologNoise * n) {this -> n = n;}
};

class rnd_control : public PrologNativeCode {
public:
	PrologNoise * n;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		// rnd_control [out]
		// rnd_control [a c]
		// rnd_control [out a c]
		// rnd_control [out bits a c]
		int vector [4];
		int vector_counter = 0;
		while (parameters -> isPair ()) {
			if (vector_counter > 3) return false;
			PrologElement * el = parameters -> getLeft ();
			if (el -> isInteger ()) vector [vector_counter++] = el -> getInteger ();
			else if (el -> isDouble ()) vector [vector_counter++] = (int) el -> getDouble ();
			else return false;
			parameters = parameters -> getRight ();
		}
		switch (vector_counter) {
		case 1: n -> control (vector [0]); return true; break;
		case 2: n -> control (vector [0], vector [1]); return true; break;
		case 3: n -> control (vector [0]); n -> control (vector [1], vector [2]); return true; break;
		case 4:
			n -> resolution (vector [1]);
			n -> control (vector [0]);
			n -> control (vector [2], vector [3]);
			return true;
			break;
		default: return false; break;
		}
		return false;
	}
	rnd_control (PrologNoise * n) {this -> n = n;}
};

typedef PrologElement * PrologElementPointer;
class series : public PrologNativeCode {
public:
	PrologNoise * n;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * e_root = parameters -> getLeft ();
		if (e_root -> isPair () || e_root -> isEarth ()) {
			PrologElement * ret = parameters -> getRight ();
			if (ret -> isPair ()) ret = ret -> getLeft ();
			int length = 0;
			while (e_root -> isPair ()) {
				e_root = e_root -> getRight ();
				length++;
			}
			if (length < 1) {ret -> setEarth (); return true;}
			PrologElementPointer * table = new PrologElementPointer [length + 32];
			length = 0;
			parameters = parameters -> getLeft ();
			while (parameters -> isPair ()) {
				table [length++] = parameters -> getLeft ();
				parameters = parameters -> getRight ();
			}
			while (length > 0) {
				int select = n -> get (0, length--);
				ret -> setPair ();
				ret -> setLeft (table [select] -> duplicate ());
				ret = ret -> getRight ();
				table [select] = table [length];
			}
			delete [] table;
			return true;
		}
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e_length = parameters -> getLeft ();
		if (! e_length -> isInteger ()) return false;
		int length = e_length -> getInteger ();
		if (length < 1) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		PrologElement * e_step = parameters -> getLeft ();
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		double root;
		if (e_root -> isInteger ()) root = (double) e_root -> getInteger ();
		else {if (! e_root -> isDouble ()) return false; root = e_root -> getDouble ();}
		double step;
		if (e_step -> isInteger ()) step = (double) e_step -> getInteger ();
		else {if (! e_step -> isDouble ()) return false; step = e_step -> getDouble ();}
		double * table = new double [length + 32];
		for (int ind = 0; ind < length; ind++) {table [ind] = root; root += step;}
		int select;
		bool double_type = e_root -> isDouble () || e_step -> isDouble ();
		while (length > 0) {
			select = n -> get (0, length--);
			parameters -> setPair ();
			if (double_type) parameters -> getLeft () -> setDouble (table [select]);
			else parameters -> getLeft () -> setInteger ((int) table [select]);
			parameters = parameters -> getRight ();
			table [select] = table [length];
		}
		delete [] table;
		return true;
	}
	series (PrologNoise * n) {this -> n = n;}
};

/////////////
// CRACKER //
/////////////

class wait : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isVar ()) {parameters -> setInteger (root -> get_system_time ()); return true;}
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (parameters -> isVar ()) {parameters -> setInteger (root -> get_system_time ()); return true;}
		if (! parameters -> isInteger ()) return false;
		root -> wait (parameters -> getInteger ());
		return true;
	}
	wait (PrologRoot * root) {this -> root = root;}
};

class timeout_class : public PrologNativeCode {
public:
	PrologRoot * root;
	// [timeout]               immediate (clears previous commands)
	// [timeout []]            clears everything
	// [timeout 127]           obvious (clears previous commands)
	// [timeout [sonda]]       immediate (clears previous commands)
	// [timeout 127 [sonda]]   obvious (clears previous commands)
	void change_query (PrologResolution * resolution, PrologElement * query) {
		PrologElement * original_query = resolution -> timeout_query;
		resolution -> timeout_query = query;
		if (original_query != NULL) delete original_query;
	}
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {
			// [timeout]               immediate (clears previous commands)
			change_query (resolution, NULL);
			resolution -> timeout = root -> get_system_time ();
			return true;
		}
		PrologElement * timeout = parameters -> getLeft ();
		if (timeout -> isEarth ()) {
			// [timeout []]            clears everything
			change_query (resolution, NULL);
			resolution -> timeout = 0;
			return true;
		}
		if (! timeout -> isInteger ()) {
			// [timeout [sonda]]       immediate (clears previous commands)
			change_query (resolution, parameters -> duplicate ());
			resolution -> timeout = root -> get_system_time ();
			return true;
		}
		parameters = parameters -> getRight ();
		// [timeout 127 [sonda]]   obvious (clears previous commands)
		change_query (resolution, parameters -> isEarth () ? NULL : parameters -> duplicate ());
		// [timeout 127]           obvious (clears previous commands)
		resolution -> timeout = root -> get_system_time () + timeout -> getInteger ();
		return true;
	}
	timeout_class (PrologRoot * root) {this -> root = root;}
};

class crack : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) return true;
		root -> start (parameters);
		return true;
	}
	crack (PrologRoot * root) {this -> root = root;}
};

#include <semaphore.h>

class semaphore_posix : public PrologNativeCode {
public:
	PrologAtom * atom;
	PrologAtom * waitAtom, * enterAtom, * signalAtom;
	sem_t semaphore;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {atom -> setMachine (0); delete this; return true;}
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologAtom * ctrl = parameters -> getAtom ();
		if (ctrl == signalAtom) {sem_post (& semaphore); return true;}
		if (ctrl == waitAtom) {sem_wait (& semaphore); return true;}
		if (ctrl == enterAtom) {return sem_trywait (& semaphore) == 0;}
		return false;
	}
	semaphore_posix (PrologAtom * atom, PrologAtom * waitAtom, PrologAtom * enterAtom, PrologAtom * signalAtom, int ind) {
		this -> atom = atom;
		this -> waitAtom = waitAtom;
		this -> enterAtom = enterAtom;
		this -> signalAtom = signalAtom;
		sem_init (& semaphore, 0, ind);
	}
	~ semaphore_posix (void) {sem_destroy (& semaphore);}
};

class semaphore_mutex : public PrologNativeCode {
public:
	PrologAtom * atom;
	PrologAtom * waitAtom, * enterAtom, * signalAtom;
	pthread_mutex_t mutex;
	pthread_cond_t conditional;
	int state;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {atom -> setMachine (0); delete this; return true;}
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologAtom * ctrl = parameters -> getAtom ();
		if (ctrl == signalAtom) {
			pthread_mutex_lock (& mutex);
			if (++state > 0) pthread_cond_broadcast (& conditional);
			pthread_mutex_unlock (& mutex);
			return true;
		}
		if (ctrl == waitAtom) {
			pthread_mutex_lock (& mutex);
			while (state <= 0) pthread_cond_wait (& conditional, & mutex);
			state--;
			pthread_mutex_unlock (& mutex);
			return true;
		}
		if (ctrl == enterAtom) {
			pthread_mutex_lock (& mutex);
			if (state <= 0) {pthread_mutex_unlock (& mutex); return false;}
			state--;
			pthread_mutex_unlock (& mutex);
			return true;
		}
		return false;
	}
	semaphore_mutex (PrologAtom * atom, PrologAtom * waitAtom, PrologAtom * enterAtom, PrologAtom * signalAtom, int ind) {
		this -> atom = atom;
		this -> waitAtom = waitAtom;
		this -> enterAtom = enterAtom;
		this -> signalAtom = signalAtom;
		state = ind;
		mutex = PTHREAD_MUTEX_INITIALIZER;
		conditional = PTHREAD_COND_INITIALIZER;
	}
	~ semaphore_mutex (void) {pthread_mutex_destroy (& mutex); pthread_cond_destroy (& conditional);}
};

class semaphore_maker : public PrologNativeCode {
public:
	PrologRoot * root;
	PrologAtom * waitAtom, * enterAtom, * signalAtom;
	bool mutexed;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (waitAtom == NULL || enterAtom == NULL || signalAtom == NULL) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * ea = parameters -> getLeft ();
		PrologAtom * atom = NULL;
		if (ea -> isVar ()) {
			atom = new PrologAtom ();
			ea -> setAtom (atom);
		} else {
			if (! ea -> isAtom ()) return false;
			atom = ea -> getAtom ();
		}
		int ind = 1;
		parameters = parameters -> getRight ();
		if (parameters -> isPair ()) {
			parameters = parameters -> getLeft ();
			if (! parameters -> isInteger ()) return false;
			ind = parameters -> getInteger ();
			if (ind < 0) return false;
		}
		PrologNativeCode * s;
		if (mutexed) s = new semaphore_mutex (atom, waitAtom, enterAtom, signalAtom, ind);
		else s = new semaphore_posix (atom, waitAtom, enterAtom, signalAtom, ind);
		if (atom -> setMachine (s)) return true;
		delete s;
		return false;
	}
	semaphore_maker (PrologRoot * root, bool mutexed = false) {
		this -> root = root;
		this -> mutexed = mutexed;
		PrologDirectory * dir = root -> searchDirectory ("studio");
		waitAtom = dir -> searchAtom ("wait");
		enterAtom = dir -> searchAtom ("enter");
		signalAtom = dir -> searchAtom ("signal");
	}
};

///////////
// MUTEX //
///////////

class PrologMutex : public PrologNativeCode {
public:
	PrologAtom * atom;
	PrologAtom * waitAtom, * enterAtom, * signalAtom;
	pthread_mutex_t mutex;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {atom -> setMachine (0); delete this; return true;}
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (! parameters -> isAtom ()) return false;
		PrologAtom * ctrl = parameters -> getAtom ();
		if (ctrl == signalAtom) {pthread_mutex_unlock (& mutex); return true;}
		if (ctrl == waitAtom) {pthread_mutex_lock (& mutex); return true;}
		if (ctrl == enterAtom) {return pthread_mutex_trylock (& mutex) == 0;}
		return false;
	}
	PrologMutex (PrologAtom * atom, PrologAtom * waitAtom, PrologAtom * enterAtom, PrologAtom * signalAtom) {
		this -> atom = atom;
		this -> waitAtom = waitAtom;
		this -> enterAtom = enterAtom;
		this -> signalAtom = signalAtom;
		mutex = PTHREAD_MUTEX_INITIALIZER;
	}
	~ PrologMutex (void) {pthread_mutex_destroy (& mutex);}
};

class MutexMaker : public PrologNativeCode {
public:
	PrologRoot * root;
	PrologAtom * waitAtom;
	PrologAtom * enterAtom;
	PrologAtom * signalAtom;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (waitAtom == 0 || enterAtom == 0 || signalAtom == 0) return false;
		if (! parameters -> isPair ()) return false;
		PrologElement * ae = parameters -> getLeft ();
		PrologAtom * atom = 0;
		if (ae -> isVar ()) {ae -> setAtom (atom = new PrologAtom ());}
		else {if (! ae -> isAtom ()) return false; atom = ae -> getAtom ();}
		PrologMutex * mutex = new PrologMutex (atom, waitAtom, enterAtom, signalAtom);
		if (atom -> setMachine (mutex)) return true;
		delete mutex;
		return false;
	}
	MutexMaker (PrologRoot * root) {
		PrologDirectory * dir = root -> searchDirectory ("studio");
		waitAtom = dir -> searchAtom ("wait");
		enterAtom = dir -> searchAtom ("enter");
		signalAtom = dir -> searchAtom ("signal");
	}
};

////////////////////////
// console procedures //
////////////////////////

class bgcolour : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (parameters -> isInteger ()) {root -> setBackground (parameters -> getInteger ()); return true;}
		if (parameters -> isVar ()) {parameters -> setInteger (root -> current_background); return true;}
		return false;
	}
	bgcolour (PrologRoot * root) {this -> root = root;}
};

class fgcolour : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isPair ()) parameters = parameters -> getLeft ();
		if (parameters -> isInteger ()) {root -> setForeground (parameters -> getInteger ()); return true;}
		if (parameters -> isVar ()) {parameters -> setInteger (root -> current_foreground); return true;}
		return false;
	}
	fgcolour (PrologRoot * root) {this -> root = root;}
};

class open_editor : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {
			root -> openEditor ();
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * selector = parameters -> getLeft ();
		if (! selector -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (parameters -> isEarth ()) {
			root -> openEditor (selector -> getInteger ());
			return true;
		}
		PrologElement * sub_selector = parameters -> getLeft ();
		if (! sub_selector -> isInteger ()) return false;
		root -> openEditor (selector -> getInteger (), sub_selector -> getInteger ());
		return true;
	}
	open_editor (PrologRoot * root) {this -> root = root;}
};

class close_editor : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (parameters -> isEarth ()) {
			root -> closeEditor ();
			return true;
		}
		if (! parameters -> isPair ()) return false;
		PrologElement * selector = parameters -> getLeft ();
		if (! selector -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (parameters -> isEarth ()) {
			root -> closeEditor (selector -> getInteger ());
			return true;
		}
		PrologElement * sub_selector = parameters -> getLeft ();
		if (! sub_selector -> isInteger ()) return false;
		root -> closeEditor (selector -> getInteger (), sub_selector -> getInteger ());
		return true;
	}
	close_editor (PrologRoot * root) {this -> root = root;}
};

class screen_coordinates : public PrologNativeCode {
public:
	PrologRoot * root;
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (! parameters -> isPair ()) return false;
		PrologElement * x = parameters -> getLeft ();
		if (! x -> isInteger ()) return false;
		parameters = parameters -> getRight ();
		if (! parameters -> isPair ()) return false;
		parameters = parameters -> getLeft ();
		if (! parameters -> isInteger ()) return false;
		root -> setScreenCoordinates (x -> getInteger (), parameters -> getInteger ());
		return true;
	}
	screen_coordinates (PrologRoot * root) {this -> root = root;}
};

///////////////////////////
// SERIAL NUMBER SERVICE //
///////////////////////////

#include "encoder.h"

class get_volume_serial_number : public PrologNativeCode {
public:
	PrologRoot * root;
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		char serial_number [64];
		encoder e;
		e . volumize_serial (serial_number, root -> serial_number, root -> volume_id);
		parameters -> setPair (root -> text (serial_number), root -> earth ());
		return true;
	}
	get_volume_serial_number (PrologRoot * root) {this -> root = root;}
};

class security_check : public PrologNativeCode {
public:
	PrologRoot * root;
	virtual bool code (PrologElement * parameters, PrologResolution * resolution) {
		encoder e;
		return e . check_serial (root -> serial_number, root -> volume_id, root -> key, root -> serial_shift);
	}
	security_check (PrologRoot * root) {this -> root = root;}
};

///////////////////
// SERVICE CLASS //
///////////////////

void PrologStudio :: init (PrologRoot * root) {
	this -> root = root;
	stdr . setRoot (root);
}

PrologNativeCode * PrologStudio :: getNativeCode (char * name) {
	if (strcmp (name, "sum") == 0) return new sum ();
	if (strcmp (name, "add") == 0) return new add ();
	if (strcmp (name, "sub") == 0) return new sub ();
	if (strcmp (name, "times") == 0) return new times ();
	if (strcmp (name, "mult") == 0) return new mult ();
	if (strcmp (name, "div") == 0) return new division ();
	if (strcmp (name, "mod") == 0) return new mod ();
	if (strcmp (name, "and") == 0) return new logical_and ();
	if (strcmp (name, "or") == 0) return new logical_or ();
	if (strcmp (name, "xor") == 0) return new logical_xor ();

	if (strcmp (name, "abs") == 0) return new abs_operation ();
	if (strcmp (name, "cos") == 0) return new cos_operation ();
	if (strcmp (name, "degrad") == 0) return new degrad_operation ();
	if (strcmp (name, "e") == 0) return new e_operation ();
	if (strcmp (name, "exp") == 0) return new exp_operation ();
	if (strcmp (name, "log2") == 0) return new log2_operation ();
	if (strcmp (name, "log10") == 0) return new log10_operation ();
	if (strcmp (name, "ln") == 0) return new ln_operation ();
	if (strcmp (name, "log") == 0) return new log_operation ();
	if (strcmp (name, "pi") == 0) return new pi_operation ();
	if (strcmp (name, "pow") == 0) return new pow_operation ();
	if (strcmp (name, "sin") == 0) return new sin_operation ();
	if (strcmp (name, "tan") == 0) return new tan_operation ();
	if (strcmp (name, "trunc") == 0) return new trunc_operation ();

	if (strcmp (name, "timestamp") == 0) return new timestamp ();

	if (strcmp (name, "is_atom") == 0) return new is_atom ();
	if (strcmp (name, "is_integer") == 0) return new is_integer ();
	if (strcmp (name, "is_double") == 0) return new is_double ();
	if (strcmp (name, "is_number") == 0) return new is_number ();
	if (strcmp (name, "is_var") == 0) return new is_var ();
	if (strcmp (name, "is_head") == 0) return new is_head ();
	if (strcmp (name, "is_text") == 0) return new is_text ();
	if (strcmp (name, "has_machine") == 0) return new has_machine (root);
	if (strcmp (name, "text_list") == 0) return new text_list ();
	if (strcmp (name, "text_term") == 0) return new text_term (root);
	if (strcmp (name, "e32") == 0) return new e32 ();
	if (strcmp (name, "less") == 0) return new less ();
	if (strcmp (name, "less_eq") == 0) return new less_eq ();
	if (strcmp (name, "greater") == 0) return new greater ();
	if (strcmp (name, "greater_eq") == 0) return new greater_eq ();
	if (strcmp (name, "max") == 0) return new max_class ();
	if (strcmp (name, "min") == 0) return new min_class ();
	if (strcmp (name, "set_uap32_captions") == 0) return new set_uap32_captions (root);
	if (strcmp (name, "set_standard_captions") == 0) return new set_standard_captions (root);
	if (strcmp (name, "set_edinburg_captions") == 0) return new set_edinburg_captions (root);
	if (strcmp (name, "set_marseille_captions") == 0) return new set_marseille_captions (root);
	if (strcmp (name, "set_functional_captions") == 0) return new set_functional_captions (root);
	if (strcmp (name, "auto_atoms") == 0) return new auto_atoms (root);
	if (strcmp (name, "scripted_atoms") == 0) return new scripted_atoms (root);
	if (strcmp (name, "pr") == 0) return new pr (& stdr);
	if (strcmp (name, "read") == 0) return new read (& stdr);
	if (strcmp (name, "readln") == 0) return new readln (& stdr);
	if (strcmp (name, "pp") == 0) return new pp (root);
	if (strcmp (name, "pt") == 0) return new pt (root);
	if (strcmp (name, "write") == 0) return new write (root);
	if (strcmp (name, "nl") == 0) return new nl (root);
	if (strcmp (name, "list") == 0) return new list (root);
	if (strcmp (name, "CL") == 0) return new CL (root);
	if (strcmp (name, "addcl") == 0) return new addcl (root);
	if (strcmp (name, "addcl0") == 0) return new addcl0 (root);
	if (strcmp (name, "delcl") == 0) return new delcl (root);
	if (strcmp (name, "overwrite") == 0) return new overwrite (root);
	if (strcmp (name, "create_atom") == 0) return new create_atom (root);
	if (strcmp (name, "search_atom") == 0) return new search_atom (root);
	if (strcmp (name, "preprocessor") == 0) return new preprocessor (root);
	if (strcmp (name, "prompt") == 0) return new prompt (root);
	if (strcmp (name, "query_stack") == 0) return new query_stack (root);
	if (strcmp (name, "object_counter") == 0) return new object_counter_class ();
	if (strcmp (name, "crack") == 0) return new crack (root);
	if (strcmp (name, "wait") == 0) return new wait (root);
	if (strcmp (name, "timeout") == 0) return new timeout_class (root);
	if (strcmp (name, "semaphore") == 0) return new semaphore_maker (root);
	if (strcmp (name, "msemaphore") == 0) return new semaphore_maker (root, true);
	if (strcmp (name, "mutex") == 0) return new MutexMaker (root);
	if (strcmp (name, "file_writer") == 0) return new file_writer (root);
	if (strcmp (name, "file_reader") == 0) return new file_reader (root);
	if (strcmp (name, "import_loader") == 0) return new import_loader (root);
	if (strcmp (name, "load_loader") == 0) return new load_loader (root);
	if (strcmp (name, "consult_loader") == 0) return new consult_loader (root);
	if (strcmp (name, "remove_module") == 0) return new remove_module (root);
	if (strcmp (name, "set_machine") == 0) return new set_machine (root);
	if (strcmp (name, "create_module") == 0) return new create_module (root);
	if (strcmp (name, "add_search_directory") == 0) return new add_search_directory (root);
	if (strcmp (name, "search_directories") == 0) return new search_directories (root);
	if (strcmp (name, "cd") == 0) return new cd (root);
	if (strcmp (name, "DIR") == 0) return new DIR (root);
	if (strcmp (name, "ARGS") == 0) return new ARGS (root);
	if (strcmp (name, "edit") == 0) return new edit (root);
	if (strcmp (name, "execute") == 0) return new execute (root);
	if (strcmp (name, "make_directory") == 0) return new make_directory (root);
	if (strcmp (name, "erase") == 0) return new erase_file (root);
	if (strcmp (name, "erase_directory") == 0) return new erase_directory (root);
	if (strcmp (name, "move") == 0) return new move_file (root);
	if (strcmp (name, "copy") == 0) return new copy_file (root);

	if (strcmp (name, "rnd") == 0) return new rnd (& n);
	if (strcmp (name, "rnd_control") == 0) return new rnd_control (& n);
	if (strcmp (name, "series") == 0) return new series (& n);

	if (strcmp (name, "CONSTANT") == 0) return new CONSTANT ();
	if (strcmp (name, "VARIABLE") == 0) return new VARIABLE ();
	if (strcmp (name, "ACCUMULATOR") == 0) return new ACCUMULATOR ();
	if (strcmp (name, "ARRAY") == 0) return new ARRAY ();

	if (strcmp (name, "background") == 0) return new bgcolour (root);
	if (strcmp (name, "foreground") == 0) return new fgcolour (root);
	if (strcmp (name, "open_editor") == 0) return new open_editor (root);
	if (strcmp (name, "close_editor") == 0) return new close_editor (root);
	if (strcmp (name, "screen_coordinates") == 0) return new screen_coordinates (root);

	if (strcmp (name, "get_volume_serial_number") == 0) return new get_volume_serial_number (root);
	if (strcmp (name, "security_check") == 0) return new security_check (root);

	return NULL;
}

