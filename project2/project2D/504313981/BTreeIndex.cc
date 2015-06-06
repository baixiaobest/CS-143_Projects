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
BTreeIndex::BTreeIndex(): nodeVec(&pf)
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
        if(pf.open(indexname, 'r')!=0 && read) return RC_FILE_OPEN_FAILED;
        for (int i=0; i<pf.endPid(); i++) {
            nodeVec.fakeRead();
        }
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
    int newKey, newPagePtr;
    return recursiveInsert(key, rid, rootPid, -1, newKey, newPagePtr);
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


RC BTreeIndex::recursiveInsert(int key, const RecordId& rid, int myPid, int parentPid, int& newKey, int& newPagePtr){
    //BASE CASE: Leaf Node Insert
    //try to insert to leaf node, if success, return 0
    
    //if insertion fails, insert and split, create new sibling node
    //and push new node to nodeVec
    //return and indicate to caller that insertion has overflow.
    //also tell caller that key and pid of new sibling node
    
    //if you are inserting to empty tree, make the node as
    //the leaf, and insert the value to leaf directly
    
    if(nodeVec.size()==0){ //in the case of index is empty
        BTLeafNode newNode;
        newNode.setNextNodePtr(-1);
        newNode.insert(key, rid);
        nodeVec.push_back(newNode);
        return 0;
    }
    
    if (nodeVec[myPid].isLeaf()) {
        BTLeafNode currentNode(nodeVec[myPid]);
        RC status = currentNode.insert(key, rid);
        if (status==RC_NODE_FULL ) {
            //non-root leaf node
            if(nodeVec.size()>1){
                BTLeafNode sibling;
                currentNode.insertAndSplit(key, rid, sibling, newKey);
                newPagePtr = nodeVec.size();
                sibling.setNextNodePtr(currentNode.getNextNodePtr());
                currentNode.setNextNodePtr(newPagePtr);
                nodeVec.push_back(sibling);
                nodeVec(myPid) = currentNode;
                return RC_NODE_FULL;
            }else{
                //insertion for root leaf node, should take special care
                BTLeafNode sibling;
                currentNode.insertAndSplit(key, rid, sibling, newKey);
                BTNonLeafNode root;
                root.initializeRoot(1, newKey, 2);
                currentNode.setNextNodePtr(2);
                nodeVec(0) = root;
                sibling.setNextNodePtr(-1);
                nodeVec.push_back(currentNode);
                nodeVec.push_back(sibling);
                return 0;
            }
        }
        nodeVec(myPid) = currentNode;
        return 0;
    }else{
    
    
        //STEP CASE: Non Leaf Node Insert
        //recursively call itself, by passing next pointer that points
        //to next node.
        
        BTNonLeafNode currentNode(nodeVec[myPid]);
        int childPid;
        int childNewKey;
        int childNewPagePtr;
        currentNode.locateChildPtr(key, childPid);
        RC status = recursiveInsert(key, rid, childPid, myPid, childNewKey, childNewPagePtr);
    
        //If returned status from calle notifies the overflow,
        //get the returned key and new sibiling node pointer and
        //try to insert to itself
        if(status==RC_NODE_FULL){
    
            status = currentNode.insert(childNewKey, childNewPagePtr);
            
            //If insertion to it self fails, then two cases
            //will be considered
            if(status == RC_NODE_FULL){
                //Case 1: current node is root node, insert the new key to
                //itself and split into two new nodes and a middle key.
                //initialize the root with middle key and put two new nodes
                //to the back of nodeVec. Make sure pointers of root are correct.
                if(myPid == rootPid){
                    BTNonLeafNode sibling, root;
                    int midKey;
                    currentNode.insertAndSplit(childNewKey, childNewPagePtr, sibling, midKey);
                    root.initializeRoot(nodeVec.size(), midKey, nodeVec.size()+1);
                    nodeVec(rootPid) = root;
                    nodeVec.push_back(currentNode);
                    nodeVec.push_back(sibling);
                    return 0;
                }
                //Case 2: current node is not root node, simply insert and split
                //the current node into two nodes and middle key, push the sibling
                //node to the nodeVec, and notify the parent insertion has overflow,
                //also pass the key and pid of new node to caller(parent).
                else{
                    BTNonLeafNode sibling;
                    int midKey;
                    currentNode.insertAndSplit(childNewKey, childNewPagePtr, sibling, midKey);
                    newPagePtr = nodeVec.size();
                    newKey = midKey;
                    nodeVec.push_back(sibling);
                    nodeVec(myPid) = currentNode;
                    return RC_NODE_FULL;
                }
            }
            nodeVec(myPid) = currentNode;
            return 0;
        }else{
            return 0;
        }
    }
}
