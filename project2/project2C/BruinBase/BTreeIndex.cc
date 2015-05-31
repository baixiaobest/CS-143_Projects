/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = 0;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
    read = false;
    write = false;
    switch (mode) {
        case 'r':
        case 'R':
            read = true;
            break;
        case 'w':
        case 'W':
            write = true;
            break;
        default:
            break;
    }
    if (read || write) {
        pf.open(indexname, 'r');
        for (int i=0; i<pf.endPid(); i++) {
            SuperNode sNode;
            sNode.read(i,pf);
            if (sNode.isLeaf()) {
                BTLeafNode newNode(sNode);
                nodeVec.push_back(newNode);
            }else{
                BTNonLeafNode newNode(sNode);
                nodeVec.push_back(newNode);
            }
        }
        pf.close();
    }
    if(write){
        pf.open(indexname, 'w');
    }
    if(!write && !read){
        return RC_INVALID_FILE_FORMAT; //invalid mode
    }
    
    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    if(write){
        for (int i=0; i<nodeVec.size(); i++) {
            nodeVec[i].write(i, pf);
        }
    }
    return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    return 0;
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    return recursiveLocate(searchKey, rootPid, cursor);
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    //readForward is for leaf
    if(!nodeVec[cursor.pid].isLeaf()) return RC_INVALID_CURSOR;
    BTLeafNode leaf(nodeVec[cursor.pid]);
    RC status = leaf.readEntry(cursor.eid, key, rid);
    // not the last key in node
    if(cursor.eid < leaf.getKeyCount()-1){
        cursor.eid++;
    }else{ //last key in node
        int nextPage = leaf.getNextNodePtr();
        if(nextPage==-1)
            return RC_END_OF_TREE;
        cursor.pid = nextPage;
        cursor.eid = 0;
    }
    return status;
}



RC BTreeIndex::recursiveLocate(int searchKey, int pid, IndexCursor& cursor){
    if(nodeVec[pid].isLeaf()){
        cursor.pid = pid;
        BTLeafNode current(nodeVec[pid]);
        return current.locate(searchKey, cursor.eid);
    }
    int nextPid;
    BTNonLeafNode current(nodeVec[pid]);
    current.locateChildPtr(searchKey, nextPid);
    return recursiveLocate(searchKey, nextPid, cursor);
}



