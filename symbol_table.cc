#include "symbol_table.h"

Symbol_Table::Symbol_Table()
{
}


Symbol_Table::~Symbol_Table()
{
}


void Symbol_Table::install (const string *id, const string *env, 
			    const expr_type t)
{
  /* Install an identifier from environment env with type t into
     symbol table.  Does not check for duplicates. */
  STAB_ENTRY *new_entry = new STAB_ENTRY;
  new_entry->id = *id;
  new_entry->env = *env;
  new_entry->position = -1;
  new_entry->type = t;
  stab.push_back (*new_entry);
  // cout << "Id: " << *id << ", Enviornment: " << *env << endl << 
  // " Position: " << new_entry->position <<", type: " << *type_to_string(t) << endl;
}


void Symbol_Table::install (const string *id, const string *env, 
				 const expr_type t, const int pos)
{
  /* Install an identifier from environment env with type t into
     symbol table.  Does not check for duplicates. */

  STAB_ENTRY *new_entry = new STAB_ENTRY;
  new_entry->id = *id;
  new_entry->env = *env;
  new_entry->position = pos;
  new_entry->type = t;
  stab.push_back (*new_entry);
  // cout << "Id: " << *id << ", Enviornment: " << *env << endl << " Position: " << pos <<", type: " << *type_to_string(t) << endl;
}

bool Symbol_Table::is_decl (const string *id, const string *env)
{
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->id.compare(*id) == 0 && it->env.compare(*env) == 0) {
      return true;
    }
  }
  return false;
}


expr_type Symbol_Table::get_type (string *id, string *env)
{
  // Return the type of identifier id of environment env.  Results in
  // garbage garbage type if (*id, *env) are not in the table.
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->id.compare(*id) == 0 && it->env.compare(*env) == 0) {
      return it->type;
    }
  }
  
  return GARBAGE_T;
}


expr_type Symbol_Table::get_type (string *proc_id, string *environment, const int pos)
{
  /* Get the type of the formal parameter in the indicated position of
     the procedure proc_id declared in the indicated environment. */
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->id.compare(*proc_id) == 0
	&& it->env.compare(*environment) == 0
	&& it->position == pos) {
      return it->type;
    }
  }

  return GARBAGE_T;
}

void Symbol_Table::update_type (expr_type standard_type_type)
{
  /* Change the type of all symbol table variables with type UNKNOWN_T
     to standard_type_type. */
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if (it->type == UNKNOWN_T) {
      it->type = standard_type_type;
      // cout << "Type updated for Id: " << it->id << ", Enviornment: " << it->env << endl << " Position: " 
      // << it->position << ", Type: " << *type_to_string(it->type) << endl;
    }
  }
}

void Symbol_Table::emit_data_directives()
{
  Emitter* e = new Emitter();
  string someID;
  vector<STAB_ENTRY>::iterator it;
  for (it = stab.begin(); it != stab.end(); ++it) {
    if(it->type == INT_T || it->type == BOOL_T){
      someID = it->id;
      e->emit_data_directive(&someID, 1);
    }
  }
  delete(e);
}

string *Symbol_Table::type_to_string (const expr_type t) const
{
  string *result;

  switch (t) {
  case INT_T:
    result = new string ("INT_T");
    break;
  case BOOL_T:
    result = new string ("BOOL_T");
    break;
  case PROCEDURE_T:
    result = new string ("PROCEDURE_T");
    break;
  case PROGRAM_T:
    result = new string ("PROGRAM_T");
    break;
  case NO_T:
    result = new string ("NO_T");
    break;
  case UNKNOWN_T:
    result = new string ("UNKNOWN_T");
    break;
  default:
    result = new string ("GARBAGE_T");
    break;
  }

  return result;
}
