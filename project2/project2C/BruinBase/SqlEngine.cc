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
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

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
  BTreeIndex indexFile;
  bool indexExists = true;
  if(indexFile.open(table+".idx", 'r')!=0) indexExists=false;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  // scan the table file from the beginning if index file does not exist
  if(!indexExists){
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
  }else{
      int startingKey=0;
      int endingKey=-1;
      vector<int> equalKeyVec;
      vector<int> notEqualKeyVec;
      vector<char*> comparingStr;
      vector<SelCond::Comparator> comp;
      for (int i=0; i<cond.size(); i++) {
          if(cond[i].attr==1){ //condition on key
              if (cond[i].comp==SelCond::GT && (atoi(cond[i].value)+1) > startingKey) {
                  startingKey = atoi(cond[i].value)+1;
              }
              if (cond[i].comp==SelCond::GE && atoi(cond[i].value) > startingKey) {
                  startingKey = atoi(cond[i].value);
              }
              if (cond[i].comp==SelCond::LT && (endingKey==-1 || (atoi(cond[i].value)-1) < endingKey)) {
                  endingKey = stoi(cond[i].value)-1;
              }
              if (cond[i].comp==SelCond::LE && (endingKey==-1 || atoi(cond[i].value) < endingKey)) {
                  endingKey = stoi(cond[i].value);
              }
              if(cond[i].comp==SelCond::EQ){
                  equalKeyVec.push_back(stoi(cond[i].value));
              }
          }else{ //condition on value
              comparingStr.push_back(cond[i].value);
              comp.push_back(cond[i].comp);
          }
      }
      for (int k=0; k<equalKeyVec.size(); k++) {
          if(equalKeyVec[k]<startingKey || (equalKeyVec[k]>endingKey && endingKey!=-1))
              goto exit_select;
      }
      //locate the starting point and start reading from there
      IndexCursor cursor;
      int key;
      RecordId rid;
      count = 0;
      indexFile.locate(startingKey, cursor);
      while(true){
          std::string value;
          bool endOfTree = ((indexFile.readForward(cursor, key, rid))==RC_END_OF_TREE);
          if(key < startingKey)
              goto next_one;
          if(endingKey!=-1 && key>endingKey)
              break;
          if (equalKeyVec.size()!=0){
              bool discardCurrentKey = false;
              for (int j=0; j<equalKeyVec.size(); j++) {
                if(key!=equalKeyVec[j])
                    discardCurrentKey = true;
              }
              if(discardCurrentKey){
                   goto next_one;
              }
          }
          switch (attr) {
              case 1:  // SELECT key
                  fprintf(stdout, "%d\n", key);
                  break;
              case 2:  // SELECT value
                  rf.read(rid, key, value);
                  fprintf(stdout, "%s\n", value.c_str());
                  break;
              case 3:  // SELECT *
                  rf.read(rid, key, value);
                  fprintf(stdout, "%d '%s'\n", key, value.c_str());
                  break;
          }
          count++;
          next_one:
          if (endOfTree) {
              break;
          }
      }
      indexFile.close();
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
  BTreeIndex indexFile;

  //create a new table file, open will automatically create it
  if ((rc = rf.open(table + ".tbl", 'w')) < 0) {
    fprintf(stderr, "Error: cannot create %s\n", table.c_str());
    return rc;
  }
  rid.pid = rid.sid = 0;

  //get every line from the input file and parse them
  if(inputfile.is_open()){
    if(index){
      if(indexFile.open(table+".idx", 'w')!=0)fprintf(stdout, "Error open index\n");
    }
    while(getline(inputfile, line)){
      int key;
      string value;
      rid = rf.endRid();
      parseLoadLine(line, key, value);
      rf.append(key, value, rid);
      indexFile.insert(key, rid);
    }
    if(index){
        if(indexFile.close()!=0)fprintf(stdout, "Error closing index\n");
    }
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
