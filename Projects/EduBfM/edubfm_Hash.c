/******************************************************************************/
/*                                                                            */
/*    Copyright (c) 2013-2015, Kyu-Young Whang, KAIST                         */
/*    All rights reserved.                                                    */
/*                                                                            */
/*    Redistribution and use in source and binary forms, with or without      */
/*    modification, are permitted provided that the following conditions      */
/*    are met:                                                                */
/*                                                                            */
/*    1. Redistributions of source code must retain the above copyright       */
/*       notice, this list of conditions and the following disclaimer.        */
/*                                                                            */
/*    2. Redistributions in binary form must reproduce the above copyright    */
/*       notice, this list of conditions and the following disclaimer in      */
/*       the documentation and/or other materials provided with the           */
/*       distribution.                                                        */
/*                                                                            */
/*    3. Neither the name of the copyright holder nor the names of its        */
/*       contributors may be used to endorse or promote products derived      */
/*       from this software without specific prior written permission.        */
/*                                                                            */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     */
/*    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       */
/*    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       */
/*    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          */
/*    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,    */
/*    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        */
/*    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER        */
/*    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT      */
/*    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN       */
/*    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         */
/*    POSSIBILITY OF SUCH DAMAGE.                                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*    ODYSSEUS/EduCOSMOS Educational Purpose Object Storage System            */
/*    (Version 1.0)                                                           */
/*                                                                            */
/*    Developed by Professor Kyu-Young Whang et al.                           */
/*                                                                            */
/*    Advanced Information Technology Research Center (AITrc)                 */
/*    Korea Advanced Institute of Science and Technology (KAIST)              */
/*                                                                            */
/*    e-mail: odysseus.educosmos@gmail.com                                    */
/*                                                                            */
/******************************************************************************/
/*
 * Module: edubfm_Hash.c
 *
 * Description:
 *  Some functions are provided to support buffer manager.
 *  Each BfMHashKey is mapping to one table entry in a hash table(hTable),
 *  and each entry has an index which indicates a buffer in a buffer pool.
 *  An ordinary hashing method is used and linear probing strategy is
 *  used if collision has occurred.
 *
 * Exports:
 *  Four edubfm_LookUp(BfMHashKey *, Four)
 *  Four edubfm_Insert(BfMHaskKey *, Two, Four)
 *  Four edubfm_Delete(BfMHashKey *, Four)
 *  Four edubfm_DeleteAll(void)
 */


#include <stdlib.h> /* for malloc & free */
#include "EduBfM_common.h"
#include "EduBfM_Internal.h"



/*@
 * macro definitions
 */  

/* Macro: BFM_HASH(k,type)
 * Description: return the hash value of the key given as a parameter
 * Parameters:
 *  BfMHashKey *k   : pointer to the key
 *  Four type       : buffer type
 * Returns: (Two) hash value
 */
#define BFM_HASH(k,type)	(((k)->volNo + (k)->pageNo) % HASHTABLESIZE(type))


/*@================================
 * edubfm_Insert()
 *================================*/
/*
 * Function: Four edubfm_Insert(BfMHashKey *, Two, Four)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BfM.
 *  For ODYSSEUS/EduCOSMOS EduBfM, refer to the EduBfM project manual.)
 *
 *  Insert a new entry into the hash table.
 *  If collision occurs, then use the linear probing method.
 *
 * Returns:
 *  error code
 *    eBADBUFINDEX_BFM - bad index value for buffer table
 */
Four edubfm_Insert(
    BfMHashKey 		*key,			/* IN a hash key in Buffer Manager */
    Two 		index,			/* IN an index used in the buffer pool */
    Four 		type)			/* IN buffer type */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four 		i;			
    Two  		hashValue;

    CHECKKEY(key);    /*@ check validity of key */
	hashValue = BFM_HASH(key, type);

    if( (index < 0) || (index > BI_NBUFS(type)) )
        ERR( eBADBUFINDEX_BFM );
	//No Collision
	if (BI_HASHTABLEENTRY(type, hashValue) == NIL) {
		BI_HASHTABLEENTRY(type,hashValue) = index;
	}
	//Collision	
	else {
		i = BI_HASHTABLEENTRY(type,hashValue);
		BI_NEXTHASHENTRY(type,index) = i;
		BI_HASHTABLEENTRY(type,hashValue) = index;
	}

    return( eNOERROR );

}  /* edubfm_Insert */



/*@================================
 * edubfm_Delete()
 *================================*/
/*
 * Function: Four edubfm_Delete(BfMHashKey *, Four)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BfM.
 *  For ODYSSEUS/EduCOSMOS EduBfM, refer to the EduBfM project manual.)
 *
 *  Look up the entry which corresponds to `key' and
 *  Delete the entry from the hash table.
 *
 * Returns:
 *  error code
 *    eNOTFOUND_BFM - The key isn't in the hash table.
 */
Four edubfm_Delete(
    BfMHashKey          *key,                   /* IN a hash key in buffer manager */
    Four                type )                  /* IN buffer type */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Two                 i, prev;                
    Two                 hashValue;


    CHECKKEY(key);    /*@ check validity of key */
	hashValue = BFM_HASH(key, type);

	if (BI_HASHTABLEENTRY(type, hashValue) == NIL) {
		return(eNOTFOUND_BFM);
	}
	else {
		i = BI_HASHTABLEENTRY(type, hashValue);
		prev = i;
		while(!EQUALKEY(key,&(BI_KEY(type,i)))){
			//printf("delete: index tracing: %d\n",i);
			if(BI_NEXTHASHENTRY(type,i) == NIL){
				return(eNOTFOUND_BFM);
			}
			prev = i;
			i = BI_NEXTHASHENTRY(type,i);
		}
		if(prev == i){
			BI_HASHTABLEENTRY(type,hashValue) = BI_NEXTHASHENTRY(type,i);
		}else{
			BI_NEXTHASHENTRY(type,prev) = BI_NEXTHASHENTRY(type,i);
		}
	}

	return(eNOERROR);

}  /* edubfm_Delete */



/*@================================
 * edubfm_LookUp()
 *================================*/
/*
 * Function: Four edubfm_LookUp(BfMHashKey *, Four)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BfM.
 *  For ODYSSEUS/EduCOSMOS EduBfM, refer to the EduBfM project manual.)
 *
 *  Look up the given key in the hash table and return its
 *  corressponding index to the buffer table.
 *
 * Retruns:
 *  index on buffer table entry holding the train specified by 'key'
 *  (NOTFOUND_IN_HTABLE - The key don't exist in the hash table.)
 */
Four edubfm_LookUp(
    BfMHashKey          *key,                   /* IN a hash key in Buffer Manager */
    Four                type)                   /* IN buffer type */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Two                 i, j;                   /* indices */
    Two                 hashValue;


    CHECKKEY(key);    /*@ check validity of key */
	hashValue = BFM_HASH(key, type);
	Four idx;
	if (BI_HASHTABLEENTRY(type, hashValue) == NIL) {
		return(NOTFOUND_IN_HTABLE);
	}
	else {
		idx = BI_HASHTABLEENTRY(type, hashValue);
		while (!EQUALKEY(&(BI_KEY(type, idx)),key)) {
			if(BI_NEXTHASHENTRY(type,idx) == NIL){
				return (NOTFOUND_IN_HTABLE);
			}
			idx = BI_NEXTHASHENTRY(type, idx);
		}
	}
	return idx;
    
    
}  /* edubfm_LookUp */



/*@================================
 * edubfm_DeleteAll()
 *================================*/
/*
 * Function: Four edubfm_DeleteAll(void)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BfM.
 *  For ODYSSEUS/EduCOSMOS EduBfM, refer to the EduBfM project manual.)
 *
 *  Delete all hash entries.
 *
 * Returns:
 *  error code
 */
Four edubfm_DeleteAll(void)
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Two 	i;
    Four        tableSize;
    Four 	m; 
	for (i = 0; i < 2; i++) {
		tableSize = HASHTABLESIZE(i);
		for (m = 0; m < tableSize; m++) {
			*(BI_HASHTABLE(i)+m) = NIL;
			//BI_HASHTABLEENTRY(i, m) = NIL;
		}
	}
    return(eNOERROR);

} /* edubfm_DeleteAll() */ 
