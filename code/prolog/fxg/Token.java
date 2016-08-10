///////////////////////////////////////////////////////////////////////////////////
//                       Copyright (C) 2016 Robert P. Wolf                       //
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

package fxg;

import Prolog . *;
import Prolog . geometry . *;

import java . io . FileWriter;

import javafx . scene . paint . Color;
import javafx . scene . canvas . GraphicsContext;

public class Token extends PrologNativeCode {
	//public String codeName () {return "FXToken";}
	//public boolean isTypeOf (String code_name) {if (codeName () == code_name) return true; return super . isTypeOf (code_name);}
	public PrologFXGStudio fxg;
	public PrologAtom atom;
	public String text = "";
	public Point rounding = new Point (0.0, 0.0);
	public Rect location = new Rect (new Point (0.0, 0.0), new Point (61.0, 61.0));
	public double rotation = 0.0;
	public Point scaling = new Point (1.0, 1.0);
	public int side = 0;
	public Rect indexing = new Rect (new Point (0.0, 0.0), new Point (4.0, 4.0));
	public boolean indexed = true;
	public Colour foreground;
	public Colour background;
	public boolean selected = false;
	public Token next;
	public Color fgcc () {return Color . color (foreground . red, foreground . green, foreground . blue, foreground . alpha);}
	public Color bgcc () {return Color . color (background . red, background . green, background . blue, background . alpha);}
	public boolean can_insert () {return false;}
	public boolean hitTest (Point position) {return selected = location . contains (position . add (location . size . half ()));}
	public void repaint () {}
	public void token_draw (GraphicsContext gc, Viewport v) {gc . save (); draw (gc, v); gc . restore ();}
	public void draw (GraphicsContext gc, Viewport v) {}
	public boolean moveOnGrid (Token token, Point position) {return false;}
	public boolean code (PrologElement parameters, PrologResolution resolution) {
		// CLOSE
		if (parameters . isEarth ()) {atom . setMachine (null); fxg . clean = false; programmatic_close (); return true;}
		if (! parameters . isPair ()) return false;
		PrologElement atom = parameters . getLeft (); parameters = parameters . getRight ();
		if (! atom . isAtom ()) return false;
		PrologAtom at = atom . getAtom ();
		// LOCATION
		if (at == fxg . location_atom) {
			if (parameters . isVar ()) {
				parameters . setPair (); parameters . getLeft () . setDouble (location . position . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (location . position . y); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (location . size . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (location . size . y); return true;
			}
			if (! parameters . isPair ()) return false; PrologElement x = parameters . getLeft (); if (! x . isNumber ()) return false; parameters = parameters . getRight ();
			if (! parameters . isPair ()) return false; PrologElement y = parameters . getLeft (); if (! y . isNumber ()) return false; parameters = parameters . getRight ();
			if (parameters . isEarth ()) {location . position = new Point (x . getNumber (), y . getNumber ()); fxg . clean = false; return true;}
			if (! parameters . isPair ()) return false; PrologElement w = parameters . getLeft (); if (! w . isNumber ()) return false; parameters = parameters . getRight ();
			if (parameters . isEarth ()) {
				location = new Rect (new Point (x . getNumber (), y . getNumber ()), new Point (w . getNumber (), w . getNumber ()));
				fxg . clean = false; sizeChanged (); return true;
			}
			if (! parameters . isPair ()) return false; PrologElement h = parameters . getLeft (); if (! h . isNumber ()) return false;
			location = new Rect (new Point (x . getNumber (), y . getNumber ()), new Point (w . getNumber (), h . getNumber ()));
			sizeChanged (); fxg . clean = false; return true;
		}
		// SIZE
		if (at == fxg . size_atom) {
			if (parameters . isVar ()) {
				parameters . setPair (); parameters . getLeft () . setDouble (location . size . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (location . size . y); return true;
			}
			if (! parameters . isPair ()) return false;
			PrologElement w = parameters . getLeft (); if (! w . isNumber ()) return false; parameters = parameters . getRight ();
			if (parameters . isEarth ()) {location . size = new Point (w . getNumber (), w . getNumber ()); sizeChanged (); fxg . clean = false; return true;}
			if (! parameters . isPair ()) return false;
			PrologElement h = parameters . getLeft (); if (! h . isNumber ()) return false;
			location . size = new Point (w . getNumber (), h . getNumber ()); sizeChanged (); fxg . clean = false; return true;
		}
		// POSITION
		if (at == fxg . position_atom) {
			if (parameters . isVar ()) {
				Point position = getPosition ();
				parameters . setPair (); parameters . getLeft () . setDouble (position . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (position . y); return true;
			}
			PrologElement x = null, y = null, att = null;
			while (parameters . isPair ()) {
				PrologElement el = parameters . getLeft ();
				if (el . isAtom ()) att = el;
				if (el . isNumber ()) {if (x == null) x = el; else y = el;}
				parameters = parameters . getRight ();
			}
			if (x == null || y == null) return false;
			Point position = new Point (x . getNumber (), y . getNumber ());
			if (att != null) {
				PrologNativeCode machine = att . getAtom () . getMachine ();
				if (machine == null) return false;
				if (! (machine instanceof Token)) return false;
				Token token = (Token) machine;
				if (moveOnGrid (token, position)) return true;
				return token . moveOnGrid (this, position);
			}
			setPosition (position);
			fxg . clean = false;
			return true;
		}
		// SCALING
		if (at == fxg . scaling_atom) {
			if (parameters . isVar ()) {
				parameters . setPair (); parameters . getLeft () . setDouble (scaling . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (scaling . y); return true;
			}
			if (! parameters . isPair ()) return false;
			PrologElement x = parameters . getLeft (); if (! x . isNumber ()) return false;
			parameters = parameters . getRight (); if (parameters . isEarth ()) {scaling . x = scaling . y = x . getNumber (); fxg . clean = false; return true;}
			if (! parameters . isPair ()) return false;
			PrologElement y = parameters . getLeft (); if (! y . isNumber ()) return false;
			scaling . x = x . getNumber (); scaling . y = y . getNumber (); fxg . clean = false; return true;
		}
		// ROTATION
		if (at == fxg . rotation_atom) {
			if (parameters . isVar ()) {parameters . setPair (); parameters . getLeft () . setDouble (rotation); return true;}
			if (! parameters . isPair ()) return false;
			PrologElement r = parameters . getLeft (); if (! r . isNumber ()) return false; rotation = r . getNumber (); fxg . clean = false; return true;
		}
		// SIDE
		if (at == fxg . side_atom) {
			if (parameters . isVar ()) {parameters . setInteger (side); return true;}
			if (! parameters . isPair ()) return false; parameters = parameters . getLeft ();
			if (! parameters . isInteger ()) return false;
			int ind = parameters . getInteger ();
			if (ind < 0 || ind >= numberOfSides ()) return false;
			side = ind; sideChanged (); fxg . clean = false; return true;
		}
		// SIDES
		if (at == fxg . sides_atom) {
			if (parameters . isPair ()) parameters = parameters . getLeft ();
			if (parameters . isVar ()) {parameters . setInteger (numberOfSides ()); return true;}
			if (! parameters . isInteger ()) return false;
			return setSides (parameters . getInteger ());
		}
		// INDEXING
		if (at == fxg . indexing_atom) {
			if (parameters . isEarth ()) {indexed = ! indexed; fxg . clean = false; return true;}
			if (parameters . isVar ()) {
				parameters . setPair (); parameters . getLeft () . setInteger ((int) indexing . position . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setInteger ((int) indexing . position . y); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setInteger ((int) indexing . size . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setInteger ((int) indexing . size . y); return true;
			}
			PrologElement el = null;
			if (parameters . isPair ()) {el = parameters . getLeft (); if (el . isNumber ()) indexing . position . x = el . getNumber (); parameters = parameters . getRight ();}
			if (parameters . isPair ()) {el = parameters . getLeft (); if (el . isNumber ()) indexing . position . y = el . getNumber (); parameters = parameters . getRight ();}
			if (parameters . isPair ()) {el = parameters . getLeft (); if (el . isNumber ()) indexing . size . x = el . getNumber (); parameters = parameters . getRight ();}
			if (parameters . isPair ()) {el = parameters . getLeft (); if (el . isNumber ()) indexing . size . y = el . getNumber ();}
			fxg . clean = false;
			return true;
		}
		if (at == fxg . indexed_atom) return indexed;
		// ROUNDING
		if (at == fxg . rounding_atom) {
			if (parameters . isVar ()) {
				parameters . setPair (); parameters . getLeft () . setDouble (rounding . x); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (rounding . y); return true;
			}
			if (! parameters . isPair ()) return false;
			PrologElement x = parameters . getLeft (); if (! x . isNumber ()) return false;
			parameters = parameters . getRight (); if (parameters . isEarth ()) {rounding . x = rounding . y = x . getNumber (); fxg . clean = false; return true;}
			if (! parameters . isPair ()) return false;
			PrologElement y = parameters . getLeft (); if (! y . isNumber ()) return false;
			rounding . x = x . getNumber (); rounding . y = y . getNumber (); fxg . clean = false; return true;
		}
		// FOREGROUND / BACKGROUND
		if (at == fxg . foreground_atom || at == fxg . background_atom) {
			if (parameters . isVar ()) {
				Colour c = at == fxg . foreground_atom ? foreground : background;
				parameters . setPair (); parameters . getLeft () . setDouble (c . red  ); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (c . green); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (c . blue ); parameters = parameters . getRight ();
				parameters . setPair (); parameters . getLeft () . setDouble (c . alpha); return true;
			}
			if (! parameters . isPair ()) return false; PrologElement red   = parameters . getLeft (); if (! red   . isNumber ()) return false; parameters = parameters . getRight ();
			if (! parameters . isPair ()) return false; PrologElement green = parameters . getLeft (); if (! green . isNumber ()) return false; parameters = parameters . getRight ();
			if (! parameters . isPair ()) return false; PrologElement blue  = parameters . getLeft (); if (! blue  . isNumber ()) return false; parameters = parameters . getRight ();
			PrologElement alpha = null;
			if (parameters . isPair ()) {alpha = parameters . getLeft (); if (! alpha . isNumber ()) return false;}
			if (at == fxg . foreground_atom) foreground = new Colour (red . getNumber (), green . getNumber (), blue . getNumber (), alpha == null ? 1.0 : alpha . getNumber ());
			else background = new Colour (red . getNumber (), green . getNumber (), blue . getNumber (), alpha == null ? 1.0 : alpha . getNumber ());
			fxg . clean = false; return true;
		}
		// SELECTING
		if (at == fxg . select_atom) {selected = true; fxg . clean = false; return true;}
		if (at == fxg . deselect_atom) {selected = false; fxg . clean = false; return true;}
		if (at == fxg . selected_atom) return selected;
		// REPAINT
		if (at == fxg . repaint_atom) {repaint (); return true;}
		return false;
	}
	/*
	bool code (PrologElement * parameters, PrologResolution * resolution) {
		if (atom -> getAtom () == roll_atom) {
			int ret = token -> randomise_side ();
			boarder_clean = false;
			if (parameters -> isEarth ()) return true;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) {parameters -> setInteger (ret); return true;}
			return false;
		}
		if (atom -> getAtom () == lock_atom) {token -> locked = true; boarder_clean = false; return true;}
		if (atom -> getAtom () == unlock_atom) {token -> locked = false; boarder_clean = false; return true;}
		if (atom -> getAtom () == is_locked_atom) {return token -> locked;}
		if (atom -> getAtom () == select_deck_atom) {if (! token -> can_insert ()) return false; board -> deck = token; return true;}
		if (atom -> getAtom () == shuffle_atom) {return token -> shuffle ();}
		if (atom -> getAtom () == insert_atom) {
			if (parameters -> isEarth ()) {
				if (board -> deck == 0) return false;
				return board -> transfer_token_to_deck (board -> deck, token);
			}
			if (! parameters -> isPair ()) return false;
			PrologElement * deck_element = parameters -> getLeft (); if (! deck_element -> isAtom ()) return false;
			PrologAtom * deck_atom = deck_element -> getAtom (); if (deck_atom == 0) return false;
			PrologNativeCode * deck_machine = deck_atom -> getMachine (); if (deck_machine == 0) return false;
			if (! deck_machine -> isTypeOf (token_actions :: name ())) return false;
			boarder_token * deck_token = ((token_actions *) deck_machine) -> token; if (deck_token == 0) return false;
			if (deck_token -> can_insert ()) return board -> transfer_token_to_deck (deck_token, token);
			if (token -> can_insert ()) return board -> transfer_token_to_deck (token, deck_token);
			return false;
		}
		if (atom -> getAtom () == release_atom) {
			boarder_token * btp = board -> release_token_from_deck (token);
			if (btp == 0) return false;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) parameters -> setAtom (btp -> atom);
			return true;
		}
		if (atom -> getAtom () == release_random_atom) {
			boarder_token * btp = board -> release_random_token_from_deck (token);
			if (btp == 0) return false;
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) parameters -> setAtom (btp -> atom);
			return true;
		}
		if (atom -> getAtom () == text_atom) {
			if (parameters -> isPair ()) parameters = parameters -> getLeft ();
			if (parameters -> isVar ()) {parameters -> setText (token -> get_text ()); return true;}
			if (! parameters -> isText ()) return false;
			return token -> set_text (parameters -> getText ());
		}
		if (atom -> getAtom () == is_deck_atom) {return token -> can_insert ();}
		return false;
	}
	*/
	public Token insert (Token token) {return null;}
	public void erase () {programmatic_close (); atom . setMachine (null); if (next != null) next . erase ();}
	public void programmatic_close () {fxg . remove_token (this);}
	public void save (FileWriter tc) {if (next != null) next . save (tc);}
	public void setPosition (Point position) {location . position = new Point (position);}
	public Point getPosition () {return new Point (location . position . x, location . position . y);}
	public void sizeChanged () {}
	public void sideChanged () {}
	public boolean setSides (int ind) {return false;}
	public int numberOfSides () {return 1;}
	public Token (PrologFXGStudio fxg, PrologAtom atom, Colour foreground, Colour background, Token next) {
		this . fxg = fxg;
		this . atom = atom;
		this . foreground = new Colour (foreground);
		this . background = new Colour (background);
		this . next = next;
	}
}
