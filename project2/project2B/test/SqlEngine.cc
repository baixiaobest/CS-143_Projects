/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  // fprintf(stdout, "Bruinbase> ");

  // // set the command line input and start parsing user input
  // sqlin = commandline;
  // sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
  //              // SqlParser.y by bison (bison is GNU equivalent of yacc)

  /*****************************LEAF NODE TEST*****************************/
  /*attr 1 is key, 2 is value, 3 is star, 4 is count*/
  
  // PageFile pf, pf2, pf3;
  // pf.open("test.index",'r');
  // BTLeafNode btLeaf, newBtLeaf;
  // if( btLeaf.read(0,pf) !=0 ) fprintf(stdout, "error reading\n");

  // RecordId newIndex; newIndex.pid=57; newIndex.sid=8;
  // int siblingKey=0;
  // int size = btLeaf.getKeyCount();
  // btLeaf.insertAndSplit(3,newIndex, newBtLeaf,siblingKey);
  // fprintf(stdout, "Split %d size node into %d and %d, with sibling key of %d\n",size, btLeaf.getKeyCount(), newBtLeaf.getKeyCount(), siblingKey);

  // int searchKey = 100; int eid; int returnedKey;
  // RecordId rid;
  // RC rc = newBtLeaf.locate(searchKey, eid);
  // newBtLeaf.readEntry(eid, returnedKey, rid);
  // fprintf(stdout, "located key %d on newBtLeaf with index %d and return code %d\n",searchKey, eid, rc);
  // fprintf(stdout, "retrieved key is %d at index %d\n", returnedKey, eid);


  // newBtLeaf.setNextNodePtr(10);
  // fprintf(stdout, "btLeaf next node is %d newBtLeaf next node is %d\n", btLeaf.getNextNodePtr(), newBtLeaf.getNextNodePtr());


  // pf2.open("result.index",'w');
  // if( btLeaf.write(0,pf2) !=0 ) fprintf(stdout, "error writting\n");

  // pf3.open("result2.index",'w');
  // if(newBtLeaf.write(0,pf3)!=0) fprintf(stdout, "error writting\n");


  

  /*************************NON LEAF NODE TEST*****************************/

  PageFile npf, npf2, npf3, npf4, npf5;
  npf.open("nonLeafTest.index", 'r');
  npf2.open("nonLeafResultInsert.txt", 'w');
  npf3.open("nonLeafResultSplit1.txt",'w');
  npf4.open("nonLeafResultSplit2.txt",'w');
  npf5.open("rootResult.txt",'w');
  BTNonLeafNode btNonLeaf, newbtNonLeaf, root;
  if(btNonLeaf.read(0,npf) != 0) fprintf(stdout, "error reading\n");

  // insertion test
  // btNonLeaf.insert(8,34);
  // btNonLeaf.insert(35,81);
  // btNonLeaf.insert(53,49);
  // if(btNonLeaf.write(0,npf2)!=0) fprintf(stdout, "error writting\n");

  //split test
  int midKey; int originalSize = btNonLeaf.getKeyCount();
  btNonLeaf.insertAndSplit(53,49,newbtNonLeaf,midKey);
  fprintf(stdout,"Split %d size node into size %d and %d with midkey %d\n", originalSize, btNonLeaf.getKeyCount(), newbtNonLeaf.getKeyCount(),midKey);
  if(btNonLeaf.write(0,npf3) != 0) fprintf(stdout, "error writting\n");
  if(newbtNonLeaf.write(0,npf4) != 0) fprintf(stdout, "error writting\n");


  //search test
  int searchKey = 1; int pid;
  btNonLeaf.locateChildPtr(searchKey, pid);
  fprintf(stdout, "search for %d returned with pid %d\n", searchKey, pid);

  searchKey = 16;
  btNonLeaf.locateChildPtr(searchKey, pid);
  fprintf(stdout, "search for %d returned with pid %d\n", searchKey, pid);

  searchKey = 19;
  btNonLeaf.locateChildPtr(searchKey, pid);
  fprintf(stdout, "search for %d returned with pid %d\n", searchKey, pid);

  searchKey = 25;
  btNonLeaf.locateChildPtr(searchKey, pid);
  fprintf(stdout, "search for %d returned with pid %d\n", searchKey, pid);


  //root test
  root.initializeRoot(1, 50, 2);
  if(root.write(0,npf5) != 0) fprintf(stdout, "error writting\n");

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  // scan the table file from the beginning
  rid.pid = rid.sid = 0;
  count = 0;
  while (rid < rf.endRid()) {
    // read the tuple
    if ((rc = rf.read(rid, key, value)) < 0) {
      fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
      goto exit_select;
    }

    // check the conditions on the tuple
    for (unsigned i = 0; i < cond.size(); i++) {
      // compute the difference between the tuple value and the condition value
      switch (cond[i].attr) {
      case 1:
	diff = key - atoi(cond[i].value);
	break;
      case 2:
	diff = strcmp(value.c_str(), cond[i].value);
	break;
      }

      // skip the tuple if any condition is not met
      switch (cond[i].comp) {
      case SelCond::EQ:
	if (diff != 0) goto next_tuple;
	break;
      case SelCond::NE:
	if (diff == 0) goto next_tuple;
	break;
      case SelCond::GT:
	if (diff <= 0) goto next_tuple;
	break;
      case SelCond::LT:
	if (diff >= 0) goto next_tuple;
	break;
      case SelCond::GE:
	if (diff < 0) goto next_tuple;
	break;
      case SelCond::LE:
	if (diff > 0) goto next_tuple;
	break;
      }
    }

    // the condition is met for the tuple. 
    // increase matching tuple counter
    count++;

    // print the tuple 
    switch (attr) {
    case 1:  // SELECT key
      fprintf(stdout, "%d\n", key);
      break;
    case 2:  // SELECT value
      fprintf(stdout, "%s\n", value.c_str());
      break;
    case 3:  // SELECT *
      fprintf(stdout, "%d '%s'\n", key, value.c_str());
      break;
    }

    // move to the next tuple
    next_tuple:
    ++rid;
  }

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  /* your code here */
  RecordFile rf;
  RC rc;
  RecordId rid;
  string line;
  ifstream inputfile(loadfile.c_str());

  //create a new table file, open will automatically create it
  if ((rc = rf.open(table + ".tbl", 'w')) < 0) {
    fprintf(stderr, "Error: cannot create %s\n", table.c_str());
    return rc;
  }
  rid.pid = rid.sid = 0;

  //get every line from the input file and parse them
  if(inputfile.is_open()){
    while(getline(inputfile, line)){
      int key;
      string value;
      rid = rf.endRid();
      parseLoadLine(line, key, value);
      rf.append(key, value, rid);
    }
    fprintf(stdout, "Finished loading data from %s to %s\n", loadfile.c_str(),table.c_str());
  }else{
    fprintf(stdout, "Failed to open file for read\n");
    return RC_FILE_OPEN_FAILED;
  }

  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
