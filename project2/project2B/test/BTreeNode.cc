#include "BTreeNode.h"

using namespace std;

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	RC rc;
	if((rc = pf.read(pid, buffer)) < 0) return rc;
	for(int i=0; i<PageFile::PAGE_SIZE; i++){
		string pair = "";
		for(int k=i; (char) buffer[k]!=' ' && (char) buffer[k]!='X' && k<PageFile::PAGE_SIZE; k++){
			pair += buffer[k];
			i++;
		}
		if( (char)buffer[i] == 'X'){
			int value = 0;
			for(int j=0; j<pair.size(); j++){
				value *= 16;
				value += ((int)pair[j]-48);
			}
			nextPage = value;
			break;
		}
		else{
			record_key_pair data;
			//hexadecimal decoding method
			data.rid.pid = ((int)pair[0]-48)*256 + ((int)pair[1]-48)*16 + ((int)pair[2]-48);
			data.rid.sid = ((int)pair[4]-48)*16 + ((int)pair[5]-48);
			data.key = ((int)pair[7]-48)*16*16*16 + ((int)pair[8]-48)*16*16 + ((int)pair[9]-48)*16 + ((int)pair[10]-48);
			recordKeyVec.push_back(data);
			//fprintf(stdout, "%x,%x,%x ", data.rid.pid, data.rid.sid, data.key);
		}
	}
	std::sort(recordKeyVec.begin(), recordKeyVec.end(), compare());
	return 0; 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 
	memset(buffer,0,PageFile::PAGE_SIZE);
	int bufferPtr = 0;
	std::string strBuff = "";
	for(int i=0; i<recordKeyVec.size();i++){
		std::string pidStr = intToHex(recordKeyVec[i].rid.pid, 3);
		std::string sidStr = intToHex(recordKeyVec[i].rid.sid, 2);
		std::string keyStr = intToHex(recordKeyVec[i].key, 4);
		strBuff += pidStr+','+sidStr+','+keyStr+' ';
	}
	std::string nextPageStr = intToHex(nextPage, 3) + 'X';
	strBuff += nextPageStr;
	for(int k=0; k<strBuff.size(); k++){
			buffer[bufferPtr] = strBuff[k];
			bufferPtr++;
	}

	RC rc;
	if((rc = pf.write(pid, buffer)) < 0) return rc;
	return 0; 
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
	keyCount = recordKeyVec.size();
	return keyCount; 
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ 
	if(keyCount == MAX_KEY) return RC_NODE_FULL;
	record_key_pair newIndex;
	newIndex.rid = rid;
	newIndex.key = key;
	recordKeyVec.push_back(newIndex);
	std::sort(recordKeyVec.begin(), recordKeyVec.end(), compare());
	keyCount = recordKeyVec.size();
	return 0; 
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	if(keyCount<MAX_KEY || sibling.getKeyCount()!=0) return RC_NODE_FULL;

	record_key_pair newIndex; newIndex.key = key; newIndex.rid = rid;
	recordKeyVec.push_back(newIndex);
	int middle = (int)(MAX_KEY/2.0+0.5);
	std::sort(recordKeyVec.begin(), recordKeyVec.end(), compare());
	for(int i= middle+1; i<recordKeyVec.size(); i++){
		sibling.insert(recordKeyVec[i].key, recordKeyVec[i].rid);
	}
	siblingKey = recordKeyVec[middle+1].key;
	recordKeyVec.erase(recordKeyVec.begin()+middle+1, recordKeyVec.end());
	//fprintf(stdout, "split to size %d and %d key %d at index %d\n", (int)recordKeyVec.size(), sibling.getKeyCount(), siblingKey, middle+1);
	return 0; 
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	bool keyNotFound = true;
	eid = recordKeyVec.size()-1; // search key too big
	for(int i=0; i<recordKeyVec.size(); i++){
		int currentKey = recordKeyVec[i].key;
		if( currentKey == searchKey){
			keyNotFound = false;
			eid = i;
			break;
		}
		if(currentKey > searchKey){
			eid = i-1; //largest smaller search key
			eid = eid > 0 ? eid : 0;
			break;
		}
	}
	if(keyNotFound)
		return RC_NO_SUCH_RECORD;
	else
		return 0;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	if(eid >= recordKeyVec.size())
		return RC_NO_SUCH_RECORD;
	key = recordKeyVec[eid].key;
	rid = recordKeyVec[eid].rid;
	return 0; 
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	return nextPage; 
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	nextPage = pid;
	return 0; 
}

std::string intToHex(int number, int length){
	std::string str = "";
	while(number!=0){
		str = (char)(number%16+48) + str;
		number = number/16;
	}

	//add heading zeros
	if(str.size() < length){
		int diff = length - str.size();
		for(int i=0; i<diff; i++)
			str = '0'+str;
	}
	return str;
}













/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return 0; }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return 0; }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ return 0; }


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ return 0; }
