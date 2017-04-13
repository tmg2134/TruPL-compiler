//Thomas Goodman
#include "buffer.h"

Buffer::Buffer (char *filename)
{

  // Open the file and fill the buffer.

  //cout << "Intialize buffer \n"; 

  source_file.open (filename);
  if (source_file.fail()) 
  {
    // Failed to open source file.
    cerr << "Can't open source file " << *filename << endl;
    buffer_fatal_error();
  }
  //cout << "File open \n"; 
}

Buffer::~Buffer()
{
	source_file.close();
}

void Buffer::fill_buffer()
{
  //cout << "Hello World \n";
  //int i = 0; 
  c = source_file.get();
  myBuffer.push_back(c);
  while(!(source_file.eof()) && myBuffer.size() < MAX_BUFFER_SIZE)
  {   
    // If last char was whitespace or a comment, skip until a new char
    c = myBuffer.back();
    // if(is_whitespace(c))
    // { 
    while(((is_whitespace(c)) || (c == COMMENT_MARKER)) && 
          !(source_file.eof()))
    {
      remove_whitespace();
      remove_comment();
      //c = source_file.get();
    // }
    myBuffer.push_back(c); // add to the list when no more
  }
    c = source_file.get();
    remove_comment();
    //Just put in a white space if it is a variation of it
    if(is_whitespace(c))
    {
      myBuffer.push_back(SPACE);
    }else if(source_file.eof())
    {
      myBuffer.push_back('$');
      // Debugging
      // for(list<char>::iterator iter = myBuffer.begin(); 
      //     iter != myBuffer.end(); iter++)
      // {
      //   cout<<*iter;
      // }
      cout<<"\n";
    }else
    {
      myBuffer.push_back(c); //Add to end
    }
  } 
}

char Buffer::next_char()
{
  char nextChar = myBuffer.front();
  //cout << "Next Char call: "<<nextChar<<endl; 
  myBuffer.pop_front();
  return nextChar;
}

void Buffer::unread_char(char undo)
{
  //cout << "unread char \n"; 
  myBuffer.push_front(undo);
}

void Buffer::buffer_fatal_error() const
{
  cerr << "Exiting on BUFFER FATAL ERROR" << endl;
  exit (-1);
}

void Buffer::remove_comment()
{
  while(c == COMMENT_MARKER)
  {
    //cout<<"Found comment \n";
    while((c != NEW_LINE) && !(source_file.eof()))
    {
      c = source_file.get();
    }
    //cout<<"Found Newline after comment \n";
  }
}

void Buffer::remove_whitespace()
{
  while(is_whitespace(c))
  {
    c = source_file.get();
  }
}

bool Buffer::not_full(int size)
{
  return size < MAX_BUFFER_SIZE;
}

bool Buffer::empty()
{
  return myBuffer.empty();  
}