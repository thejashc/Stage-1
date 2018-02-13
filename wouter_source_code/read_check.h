  if ( File.rdstate() != 0 )
  {
    cout << "*** error reading file ***" << endl << endl;
    cout << "    state flag : " << File.rdstate() << endl;
    abort();
  }

  char    DummyChar;
  File >> DummyChar;
  if ( ! File.eof() )
  {
    cout << "*** file contains unread data ***" << endl << endl;
    abort();
  }
