//Thomas Goodman
#include "scanner.h"
#include "parser.h"


int main (int argc, char **argv)
{

  char *filename;

  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <input file name>" << endl;
    exit (-1);
  }

  filename = argv[1];
	Scanner *s = new Scanner(filename);
	Parser *p = new Parser(s);
	bool result;
	result = p->parse_program();
	if(result){
		cout << "Amazing, it worked! Parse Succeded!" << endl;
	}else{
		cout << "Seems somthing went wrong. Parse Failed" << endl;
	}

	//cout << (result) << endl;

	delete p;

}