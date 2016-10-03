﻿/*
	Copyright (c) 2012 TOSHIBA CORPORATION.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
	@file
	@brief Definition of BtreeMap
*/
#ifndef BTREE_MAP_H_
#define BTREE_MAP_H_

#include "util/trace.h"
#include "base_index.h"
#include "data_type.h"
#include "gs_error.h"
#include "object_manager.h"
#include "string_processor.h"  
#include "value.h"			   
#include "value_operator.h"
#include <iomanip>
#include <iostream>

UTIL_TRACER_DECLARE(BTREE_MAP);

class TransactionContext;
class BaseContainer;


/*!
	@brief String-type search key
*/
struct StringObject {
	StringObject(uint8_t *ptr) : ptr_(ptr) {}
	StringObject() : ptr_(NULL) {}
	uint8_t *ptr_;
};

/*!
	@brief For String-type Btree key
*/
struct StringKey {
	OId oId_;
	friend std::ostream &operator<<(std::ostream &output, const StringKey &kv) {
		output << kv.oId_;
		return output;
	}
};

/*!
	@brief a pair of (Key, Value) For Btree
*/
template <typename K, typename V>
struct KeyValue {  
	K key_;
	V value_;

	KeyValue() : key_(), value_() {}

	friend std::ostream &operator<<(
		std::ostream &output, const KeyValue<K, V> &kv) {
		output << "[" << kv.key_ << ",";
		output << kv.value_;
		output << "]";
		return output;
	}
};

static inline int32_t keyCmp(TransactionContext &txn,
	ObjectManager &objectManager, const KeyValue<StringObject, OId> &e1,
	const KeyValue<StringKey, OId> &e2) {
	StringCursor *obj1 = reinterpret_cast<StringCursor *>(e1.key_.ptr_);
	StringCursor obj2(txn, objectManager, e2.key_.oId_);
	return compareStringString(txn, obj1->str(), obj1->stringLength(),
		obj2.str(), obj2.stringLength());
}

static inline int32_t keyCmp(TransactionContext &txn,
	ObjectManager &objectManager, const KeyValue<StringKey, OId> &e1,
	const KeyValue<StringObject, OId> &e2) {
	StringCursor obj1(txn, objectManager, e1.key_.oId_);
	StringCursor *obj2 = reinterpret_cast<StringCursor *>(e2.key_.ptr_);
	return compareStringString(txn, obj1.str(), obj1.stringLength(),
		obj2->str(), obj2->stringLength());
}

template <typename K, typename V>
static inline int32_t keyCmp(TransactionContext &txn,
	ObjectManager &objectManager, const KeyValue<StringKey, OId> &e1,
	const KeyValue<StringKey, OId> &e2) {
	StringCursor obj1(txn, objectManager, e1.key_.oId_);
	StringCursor obj2(txn, objectManager, e2.key_.oId_);
	return compareStringString(
		txn, obj1.str(), obj1.stringLength(), obj2.str(), obj2.stringLength());
}

static inline int32_t keyCmp(TransactionContext &, ObjectManager &,
	const KeyValue<double, OId> &e1, const KeyValue<double, OId> &e2) {
	if (isnan(e1.key_)) {
		if (isnan(e2.key_)) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else if (isnan(e2.key_)) {
		return -1;
	}
	else {
		return e1.key_ < e2.key_ ? -1 : (e1.key_ == e2.key_ ? 0 : 1);
	}
}

static inline int32_t keyCmp(TransactionContext &, ObjectManager &,
	const KeyValue<float, OId> &e1, const KeyValue<float, OId> &e2) {
	if (isnan(e1.key_)) {
		if (isnan(e2.key_)) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else if (isnan(e2.key_)) {
		return -1;
	}
	else {
		return e1.key_ < e2.key_ ? -1 : (e1.key_ == e2.key_ ? 0 : 1);
	}
}

template <typename K, typename V>
static inline int32_t keyCmp(TransactionContext &, ObjectManager &,
	const KeyValue<K, V> &e1, const KeyValue<K, V> &e2) {
	return e1.key_ < e2.key_ ? -1 : (e1.key_ == e2.key_ ? 0 : 1);
}

static inline int32_t valueCmp(TransactionContext &txn,
	ObjectManager &objectManager, const KeyValue<StringObject, OId> &e1,
	const KeyValue<StringKey, OId> &e2) {
	StringCursor *obj1 = reinterpret_cast<StringCursor *>(e1.key_.ptr_);
	StringCursor obj2(txn, objectManager, e2.key_.oId_);
	int32_t ret = compareStringString(txn, obj1->str(), obj1->stringLength(),
		obj2.str(), obj2.stringLength());
	if (ret != 0) {
		return ret;
	}
	else {
		return e1.value_ < e2.value_ ? -1 : (e1.value_ == e2.value_ ? 0 : 1);
	}
	return 0;
}

static inline int32_t valueCmp(TransactionContext &txn,
	ObjectManager &objectManager, const KeyValue<StringKey, OId> &e1,
	const KeyValue<StringKey, OId> &e2) {
	StringCursor obj1(txn, objectManager, e1.key_.oId_);
	StringCursor obj2(txn, objectManager, e2.key_.oId_);
	int32_t ret = compareStringString(
		txn, obj1.str(), obj1.stringLength(), obj2.str(), obj2.stringLength());
	if (ret != 0) {
		return ret;
	}
	else {
		return e1.value_ < e2.value_ ? -1 : (e1.value_ == e2.value_ ? 0 : 1);
	}
	return 0;
}

static inline int32_t valueCmp(TransactionContext &, ObjectManager &,
	const KeyValue<double, OId> &e1, const KeyValue<double, OId> &e2) {
	if (isnan(e1.key_)) {
		if (isnan(e2.key_)) {
			return e1.value_ < e2.value_ ? -1
										 : (e1.value_ == e2.value_ ? 0 : 1);
		}
		else {
			return 1;
		}
	}
	else if (isnan(e2.key_)) {
		return -1;
	}
	else {
		if (e1.key_ == e2.key_) {
			return e1.value_ < e2.value_ ? -1
										 : (e1.value_ == e2.value_ ? 0 : 1);
		}
		return e1.key_ < e2.key_ ? -1 : 1;
	}
}

static inline int32_t valueCmp(TransactionContext &, ObjectManager &,
	const KeyValue<float, OId> &e1, const KeyValue<float, OId> &e2) {
	if (isnan(e1.key_)) {
		if (isnan(e2.key_)) {
			return e1.value_ < e2.value_ ? -1
										 : (e1.value_ == e2.value_ ? 0 : 1);
		}
		else {
			return 1;
		}
	}
	else if (isnan(e2.key_)) {
		return -1;
	}
	else {
		if (e1.key_ == e2.key_) {
			return e1.value_ < e2.value_ ? -1
										 : (e1.value_ == e2.value_ ? 0 : 1);
		}
		return e1.key_ < e2.key_ ? -1 : 1;
	}
}

template <typename K, typename V>
static inline int32_t valueCmp(TransactionContext &, ObjectManager &,
	const KeyValue<K, V> &e1, const KeyValue<K, V> &e2) {
	if (e1.key_ == e2.key_) {
		return e1.value_ < e2.value_ ? -1 : (e1.value_ == e2.value_ ? 0 : 1);
	}
	return e1.key_ < e2.key_ ? -1 : 1;
}

/*!
	@brief Btree Map Index
*/
class BtreeMap : public BaseIndex {
public:
	/*!
		@brief Represents the type(s) of Btree
	*/
	enum BtreeMapType {
		TYPE_SINGLE_KEY,
		TYPE_UNIQUE_RANGE_KEY,
		TYPE_UNDEF_KEY,
	};

	BtreeMap(TransactionContext &txn, ObjectManager &objectManager,
		const AllocateStrategy &strategy, BaseContainer *container = NULL)
		: BaseIndex(txn, objectManager, strategy, container),
		  nodeMaxSize_(NORMAL_MAX_ITEM_SIZE),
		  nodeMinSize_(NORMAL_MIN_ITEM_SIZE),
		  nodeBlockType_(DEFAULT_NODE_BLOCK_TYPE) {}
	BtreeMap(TransactionContext &txn, ObjectManager &objectManager, OId oId,
		const AllocateStrategy &strategy, BaseContainer *container = NULL)
		: BaseIndex(txn, objectManager, oId, strategy, container) {
		BNode<char, char> rootNode(this, allocateStrategy_);
		nodeBlockType_ = rootNode.getNodeBlockType();
		nodeMaxSize_ = NORMAL_MAX_ITEM_SIZE;
		nodeMinSize_ = NORMAL_MIN_ITEM_SIZE;
	}
	~BtreeMap() {}

	static const int32_t ROOT_UPDATE = 0x80000000;  
	static const int32_t TAIL_UPDATE = 0x40000000;  

	/*!
		@brief Represents the type(s) of dump
	*/
	enum {
		SUMMARY_HEADER_DUMP = 0,
		SUMMARY_DUMP = 1,
		ALL_DUMP = 2,
		PARAM_DUMP = 3,
		MVCC_ROW_DUMP = 4,
	};

public:
	/*!
		@brief Information related to a search
	*/
	struct SearchContext : BaseIndex::SearchContext {
		const void *startKey_;  
		uint32_t startKeySize_;  
		int32_t isStartKeyIncluded_;  
		const void *endKey_;  
		uint32_t endKeySize_;
		int32_t isEndKeyIncluded_;
		ColumnType keyType_;  
		bool isEqual_;		  

		SearchContext()
			: BaseIndex::SearchContext(),
			  startKey_(NULL),
			  startKeySize_(0),
			  isStartKeyIncluded_(true),
			  endKey_(NULL),
			  endKeySize_(0),
			  isEndKeyIncluded_(true),
			  keyType_(COLUMN_TYPE_WITH_BEGIN),
			  isEqual_(false)
		{
		}

		SearchContext(ColumnId columnId, const void *startKey,
			uint32_t startKeySize, int32_t isStartKeyIncluded,
			const void *endKey, uint32_t endKeySize, int32_t isEndKeyIncluded,
			uint32_t conditionNum, TermCondition *conditionList,
			ResultSize limit, ColumnType keyType = COLUMN_TYPE_WITH_BEGIN)
			: BaseIndex::SearchContext(
				  columnId, conditionNum, conditionList, limit),
			  startKey_(startKey),
			  startKeySize_(startKeySize),
			  isStartKeyIncluded_(isStartKeyIncluded),
			  endKey_(endKey),
			  endKeySize_(endKeySize),
			  isEndKeyIncluded_(isEndKeyIncluded),
			  keyType_(keyType),
			  isEqual_(false)
		{
		}
		SearchContext(ColumnId columnId, const void *key, uint32_t keySize,
			uint32_t conditionNum, TermCondition *conditionList,
			ResultSize limit, ColumnType keyType = COLUMN_TYPE_WITH_BEGIN)
			: BaseIndex::SearchContext(
				  columnId, conditionNum, conditionList, limit),
			  startKey_(key),
			  startKeySize_(keySize),
			  isStartKeyIncluded_(true),
			  endKey_(key),
			  endKeySize_(keySize),
			  isEndKeyIncluded_(true),
			  keyType_(keyType),
			  isEqual_(true)
		{
		}
	};

	/*!
		@brief Cursor for search
	*/
	struct BtreeCursor {
		OId nodeId_;
		int32_t loc_;
		BtreeCursor() {
			nodeId_ = UNDEF_OID;
			loc_ = 0;
		}
	};

	int32_t initialize(TransactionContext &txn, ColumnType columnType,
		bool isUnique, BtreeMapType btreeMapType);
	template <typename K, typename V>
	int32_t initialize(TransactionContext &txn, ColumnType columnType,
		bool isUnique, BtreeMapType btreeMapType);
	int32_t finalize(TransactionContext &txn);
	int32_t clear(TransactionContext &txn) {
		return finalize(txn);
	}

	int32_t insert(TransactionContext &txn, const void *key, OId oId);
	int32_t remove(TransactionContext &txn, const void *key, OId oId);
	int32_t update(
		TransactionContext &txn, const void *key, OId oId, OId newOId);
	template <typename K, typename V>
	int32_t insert(TransactionContext &txn, K &key, V &value);
	template <typename K, typename V>
	int32_t remove(TransactionContext &txn, K &key, V &value);
	template <typename K, typename V>
	int32_t update(TransactionContext &txn, K &key, V &oldValue, V &newValue);

	int32_t search(TransactionContext &txn, const void *key, uint32_t keySize,
		OId &oId);  

	int32_t search(TransactionContext &txn, SearchContext &sc,
		util::XArray<OId> &idList, OutputOrder outputOrder = ORDER_UNDEFINED);

	template <typename K, typename V, typename R>
	int32_t search(TransactionContext &txn, SearchContext &sc,
		util::XArray<R> &idList, OutputOrder outputOrder = ORDER_UNDEFINED) {
		if (isEmpty()) {
			return GS_FAIL;
		}
		return find<K, K, V, R>(txn, sc, idList, outputOrder);
	}

	int32_t getAll(
		TransactionContext &txn, ResultSize limit, util::XArray<OId> &idList);
	int32_t getAll(TransactionContext &txn, ResultSize limit,
		util::XArray<OId> &idList, BtreeCursor &cursor);
	template <typename K, typename V>
	int32_t getAll(TransactionContext &txn, ResultSize limit,
		util::XArray<std::pair<K, V> > &keyValueList);
	template <typename K, typename V>
	int32_t getAll(TransactionContext &txn, ResultSize limit,
		util::XArray<std::pair<K, V> > &keyValueList, BtreeCursor &cursor);

	inline OId getTailNodeOId() const {
		BNode<char, char> rootNode(this, allocateStrategy_);
		return rootNode.getTailNodeOId();
	}
	inline OId getTail(TransactionContext &txn) const {
		if (isEmpty()) {
			return UNDEF_OID;
		}
		else {
			BNode<Timestamp, OId> node(
				txn, *getObjectManager(), getTailNodeOId(), allocateStrategy_);
			return node.getKeyValue(node.numkeyValues() - 1).value_;
		}
	}
	static OId getTailDirect(TransactionContext &txn,
		ObjectManager &objectManager, OId tailNodeOId) {
		if (tailNodeOId == UNDEF_OID) {
			return UNDEF_OID;
		}
		else {
			BNode<Timestamp, OId> node(
				txn, objectManager, tailNodeOId, AllocateStrategy());
			if (node.numkeyValues() == 0) {
				return UNDEF_OID;
			}
			else {
				return node.getKeyValue(node.numkeyValues() - 1).value_;
			}
		}
	}

	inline bool isEmpty() const {
		BNode<char, char> rootNode(this, allocateStrategy_);
		return rootNode.numkeyValues() == 0;
	}

	std::string dump(TransactionContext &txn, uint8_t mode);
	template <typename K, typename V>
	std::string dump(TransactionContext &txn);

	std::string validate(TransactionContext &txn);

private:
	int32_t nodeMaxSize_;	
	int32_t nodeMinSize_;	
	uint8_t nodeBlockType_;  

private:
	static const int32_t INITIAL_DEFAULT_ITEM_SIZE_THRESHOLD =
		5;  
	static const int32_t INITIAL_MVCC_ITEM_SIZE_THRESHOLD =
		2;  

	static const int32_t NORMAL_MIN_ITEM_SIZE =
		30;  
	static const int32_t NORMAL_MAX_ITEM_SIZE =
		2 * NORMAL_MIN_ITEM_SIZE + 1;  
	static const uint8_t DEFAULT_NODE_BLOCK_TYPE =
		1;  

	/*!
		@brief Btree Node Header format
	*/
	struct BNodeHeader {  
		OId parent_;	  
		OId self_;		  
		OId children_;	
		int32_t size_;	
		ColumnType keyType_;
		int8_t btreeMapType_;
		uint8_t isUnique_;
		uint8_t nodeBlockType_;
		OId tailNodeOId_;
	};

	/*!
		@brief Btree Node format
	*/
	template <typename K, typename V>
	struct BNodeImage {  
		BNodeHeader header_;

		KeyValue<K, V>
			keyValues_[1];  
	};

	/*!
		@brief Object for Btree Node
	*/
	template <typename K, typename V>
	class BNode : public BaseObject {
	public:
		BNode(const BtreeMap *map, const AllocateStrategy &allocateStrategy)
			: BaseObject(map->getPartitionId(), *(map->getObjectManager())),
			  allocateStrategy_(allocateStrategy) {
			BaseObject::copyReference(UNDEF_OID, map->getBaseAddr());
		}

		BNode(TransactionContext &txn, ObjectManager &objectManager,
			const AllocateStrategy &allocateStrategy)
			: BaseObject(txn.getPartitionId(), objectManager),
			  allocateStrategy_(allocateStrategy) {}
		BNode(TransactionContext &txn, ObjectManager &objectManager, OId oId,
			const AllocateStrategy &allocateStrategy)
			: BaseObject(txn.getPartitionId(), objectManager, oId),
			  allocateStrategy_(allocateStrategy) {}

		void directLoad(const BtreeMap *map) {
			BaseObject::copyReference(UNDEF_OID, map->getBaseAddr());
		}


		inline void initialize(TransactionContext &txn, OId self, bool leaf,
			int32_t nodeMaxSize, uint8_t nodeBlockType) {
			BNodeImage<K, V> *image = getImage();
			image->header_.size_ = 0;
			image->header_.parent_ = UNDEF_OID;
			image->header_.self_ = self;
			image->header_.keyType_ = COLUMN_TYPE_WITH_BEGIN;
			image->header_.btreeMapType_ = TYPE_UNDEF_KEY;
			image->header_.isUnique_ = 0;
			image->header_.nodeBlockType_ = nodeBlockType;
			image->header_.tailNodeOId_ = UNDEF_OID;
			if (leaf) {
				image->header_.children_ = UNDEF_OID;
			}
			else {
				BaseObject childrenDirtyObject(
					txn.getPartitionId(), *getObjectManager());
				OId *childrenList = childrenDirtyObject.allocate<OId>(
					sizeof(OId) * (nodeMaxSize + 1), allocateStrategy_,
					image->header_.children_, OBJECT_TYPE_BTREE_MAP);
				for (int32_t i = 0; i < nodeMaxSize + 1; ++i) {
					childrenList[i] = UNDEF_OID;
				}
			}
		}
		inline OId getChild(TransactionContext &txn, int32_t nth) const {
			assert(getImage()->header_.children_ != UNDEF_OID);

			BaseObject baseObject(txn.getPartitionId(), *getObjectManager(),
				getImage()->header_.children_);
			OId *oIdList = baseObject.getBaseAddr<OId *>();
			return oIdList[nth];
		}
		inline void setChild(TransactionContext &txn, int32_t nth, OId oId) {
			assert(getImage()->header_.children_ != UNDEF_OID);

			UpdateBaseObject baseObject(txn.getPartitionId(),
				*getObjectManager(), getImage()->header_.children_);
			OId *oIdListDirtyObject = baseObject.getBaseAddr<OId *>();
			oIdListDirtyObject[nth] = oId;
		}
		inline void insertVal(
			TransactionContext &txn, const KeyValue<K, V> &val,
			int32_t (*cmp)(TransactionContext &txn,
				ObjectManager &objectManager, const KeyValue<K, V> &e1,
				const KeyValue<K, V> &e2)) {
			BNodeImage<K, V> *image = getImage();
			if (image->header_.size_ == 0) {
				image->keyValues_[0] = val;
			}
			else if (cmp(txn, *getObjectManager(), val,
						 getKeyValue(numkeyValues() - 1)) > 0) {
				image->keyValues_[numkeyValues()] = val;
			}
			else {
				int32_t i, j;
				for (i = 0; i < image->header_.size_; ++i) {
					if (cmp(txn, *getObjectManager(), val,
							image->keyValues_[i]) < 0) {
						for (j = image->header_.size_; j > i; --j) {
							image->keyValues_[j] = image->keyValues_[j - 1];
						}
						image->keyValues_[i] = val;
						break;
					}
				}
			}
			image->header_.size_++;
		}

		inline void removeVal(int32_t m) {
			BNodeImage<K, V> *image = getImage();
			for (int32_t i = m; i < image->header_.size_ - 1; ++i) {
				image->keyValues_[i] = image->keyValues_[i + 1];
			}
			image->header_.size_--;
		}

		inline void insertChild(
			TransactionContext &txn, int32_t m, BNode<K, V> &dirtyNode) {
			BNodeImage<K, V> *image = getImage();
			assert(image->header_.children_ != UNDEF_OID);

			UpdateBaseObject baseObject(txn.getPartitionId(),
				*getObjectManager(), image->header_.children_);
			OId *oIdListDirtyObject = baseObject.getBaseAddr<OId *>();
			for (int32_t i = numkeyValues(); i >= m; --i) {
				oIdListDirtyObject[i + 1] = oIdListDirtyObject[i];
			}
			oIdListDirtyObject[m] = dirtyNode.getSelfOId();
			dirtyNode.setParentOId(image->header_.self_);
		}
		inline void removeChild(TransactionContext &txn, int32_t m) {
			BNodeImage<K, V> *image = getImage();
			assert(image->header_.children_ != UNDEF_OID);
			UpdateBaseObject baseObject(txn.getPartitionId(),
				*getObjectManager(), image->header_.children_);
			OId *oIdListDirtyObject = baseObject.getBaseAddr<OId *>();
			for (int32_t i = m; i < image->header_.size_; ++i) {
				oIdListDirtyObject[i] = oIdListDirtyObject[i + 1];
			}
			oIdListDirtyObject[image->header_.size_] = UNDEF_OID;
		}
		inline bool isLeaf() const {
			return (getImage()->header_.children_ == UNDEF_OID ? true : false);
		}
		inline bool isRoot() const {
			return (getImage()->header_.parent_ == UNDEF_OID ? true : false);
		}
		inline int32_t numkeyValues() const {
			return getImage()->header_.size_;
		}

		inline void allocateVal(
			TransactionContext &txn, int32_t m, KeyValue<StringObject, OId> e) {
			BNodeImage<K, V> *image = getImage();
			StringCursor *stringCursor =
				reinterpret_cast<StringCursor *>(e.key_.ptr_);
			BaseObject object(txn.getPartitionId(), *getObjectManager());
			StringObject *stringObject = object.allocateNeighbor<StringObject>(
				stringCursor->getObjectSize(), allocateStrategy_,
				image->keyValues_[m].key_.oId_, image->header_.self_,
				OBJECT_TYPE_BTREE_MAP);
			memcpy(stringObject, stringCursor->data(),
				stringCursor->getObjectSize());
			image->keyValues_[m].value_ = e.value_;
			image->header_.size_++;
		}
		inline void allocateVal(
			TransactionContext &txn, int32_t m, KeyValue<K, V> e) {
			getImage()->keyValues_[m] = e;
			getImage()->header_.size_++;
		}
		inline void freeVal(TransactionContext &, int32_t m) {}

		inline void finalize(TransactionContext &txn) {
			BNodeImage<K, V> *image = getImage();
			for (int32_t i = 0; i < image->header_.size_; ++i) {
				freeVal(txn, i);
			}
			if (!isLeaf()) {
				util::XArray<OId> childOIdList(txn.getDefaultAllocator());
				util::XArray<OId>::iterator itr;
				for (int32_t i = 0; i < image->header_.size_ + 1; ++i) {
					OId childOId = getChild(txn, i);
					childOIdList.push_back(childOId);
				}
				getObjectManager()->free(
					txn.getPartitionId(), image->header_.children_);

				for (itr = childOIdList.begin(); itr != childOIdList.end();
					 itr++) {
					if (*itr != UNDEF_OID) {
						BNode<K, V> dirtyChildNode(
							txn, *getObjectManager(), allocateStrategy_);
						dirtyChildNode.load(*itr, true);
						dirtyChildNode.finalize(txn);
					}
				}
			}
			BaseObject::finalize();
		}

		inline void remove(TransactionContext &txn) {
			BNodeImage<K, V> *image = getImage();
			if (image->header_.children_ != UNDEF_OID) {
				getObjectManager()->free(
					txn.getPartitionId(), image->header_.children_);
			}
			BaseObject::finalize();
		}

		inline ColumnType getKeyType() const {
			return getImage()->header_.keyType_;
		}
		inline int8_t getBtreeMapType() const {
			return getImage()->header_.btreeMapType_;
		}
		inline uint8_t isUnique() const {
			return getImage()->header_.isUnique_;
		}
		inline uint8_t getNodeBlockType() const {
			return getImage()->header_.nodeBlockType_;
		}
		inline OId getTailNodeOId() const {
			return getImage()->header_.tailNodeOId_;
		}
		inline OId getParentOId() const {
			return getImage()->header_.parent_;
		}
		inline OId getSelfOId() const {
			return getImage()->header_.self_;
		}
		inline const KeyValue<K, V> &getKeyValue(int32_t m) const {
			return getImage()->keyValues_[m];
		}

		inline void setTailNodeOId(OId oId) {
			getImage()->header_.tailNodeOId_ = oId;
		}
		inline void setKeyValue(int32_t m, const KeyValue<K, V> &keyVal) {
			getImage()->keyValues_[m] = keyVal;
		}
		inline void setValue(int32_t m, const V &val) {
			getImage()->keyValues_[m].value_ = val;
		}
		inline void setNumkeyValues(int32_t num) {
			getImage()->header_.size_ = num;
		}
		inline void addNumkeyValues(int32_t num) {
			getImage()->header_.size_ += num;
		}
		inline void setParentOId(OId oId) const {
			getImage()->header_.parent_ = oId;
		}

		inline void setRootNodeHeader(ColumnType keyType, int8_t btreeMapType,
			uint8_t isUnique, OId oId) {
			BNodeImage<K, V> *image = getImage();
			image->header_.keyType_ = keyType;
			image->header_.btreeMapType_ = btreeMapType;
			image->header_.isUnique_ = isUnique;
			image->header_.tailNodeOId_ = oId;
		}
		inline std::string dump(TransactionContext &txn) {
			util::NormalOStringStream out;
			out << "(@@Node@@)";
			for (int32_t i = 0; i < numkeyValues(); ++i) {
				out << getKeyValue(i) << ", ";
			}
			return out.str();
		}

	private:
		const AllocateStrategy &allocateStrategy_;
		BNode(const BNode &);  
		BNode &operator=(const BNode &);  
		BNodeImage<K, V> *getImage() const {
			return reinterpret_cast<BNodeImage<K, V> *>(getBaseAddr());
		}
	};


	template <typename K, typename V>
	BNode<K, V> *getRootNodeAddr() {
		return reinterpret_cast<BNode<K, V> *>(this);
	}

	template <typename K, typename V>
	const BNode<K, V> *getRootNodeAddr() const {
		return reinterpret_cast<const BNode<K, V> *>(this);
	}


	template <typename K, typename V>
	static int32_t getInitialNodeSize() {
		return static_cast<int32_t>(
			sizeof(BNodeHeader) +
			sizeof(KeyValue<K, V>) * getInitialItemSizeThreshold<K, V>());
	}

	template <typename K, typename V>
	static int32_t getInitialItemSizeThreshold();

	template <typename K, typename V>
	inline int32_t getNormalNodeSize() {
		return static_cast<int32_t>(
			sizeof(BNodeHeader) + sizeof(KeyValue<K, V>) * nodeMaxSize_);
	}

	inline ColumnType keyType() const {
		BNode<char, char> rootNode(this, allocateStrategy_);
		return rootNode.getKeyType();
	}
	inline MapType getBtreeMapType() const {
		BNode<char, char> rootNode(this, allocateStrategy_);
		return rootNode.getBtreeMapType();
	}
	template <typename K, typename V>
	std::string getTreeStatus();

private:
	template <typename K, typename V>
	void getHeadNode(TransactionContext &txn, BNode<K, V> &baseHeadNode) {
		getRootBNodeObject<K, V>(baseHeadNode);
		while (1) {
			if (baseHeadNode.isLeaf()) {
				break;
			}
			OId nodeId = baseHeadNode.getChild(txn, 0);
			if (nodeId != UNDEF_OID) {
				baseHeadNode.load(nodeId);
			}
			else {
				break;
			}
		}
	}

	template <typename P, typename K, typename V>
	bool findNode(TransactionContext &txn, KeyValue<P, V> &val,
		BNode<K, V> &node, int32_t &loc,
		int32_t (*keyCmpLeft)(TransactionContext &txn,
					  ObjectManager &objectManager, const KeyValue<P, V> &e1,
					  const KeyValue<K, V> &e2)) {
		if (isEmpty()) {
			node.reset();  
			loc = 0;
			return false;
		}
		else {
			bool isUniqueKey = isUnique();
			int32_t size;
			getRootBNodeObject<K, V>(node);
			BNode<K, V> *currentNode = &node;
			while (1) {
				OId nodeId;
				size = currentNode->numkeyValues();
				if (keyCmpLeft(txn, *getObjectManager(), val,
						currentNode->getKeyValue(size - 1)) > 0) {
					if (currentNode->isLeaf()) {
						loc = size;
						return false;
					}
					else {
						nodeId = currentNode->getChild(txn, size);
					}
				}
				else {
					int32_t l = 0, r = size;
					while (l < r) {
						int32_t i = (l + r) >> 1;
						int32_t currentCmpResult =
							keyCmpLeft(txn, *getObjectManager(), val,
								currentNode->getKeyValue(i));
						if (currentCmpResult > 0) {
							l = i + 1;
						}
						else if (isUniqueKey && currentCmpResult == 0) {
							loc = i;
							return true;
						}
						else {
							r = i;
						}
					}
					assert(r == l);
					int32_t cmpResult = keyCmpLeft(txn, *getObjectManager(),
						val, currentNode->getKeyValue(l));
					if (cmpResult < 0) {
						if (currentNode->isLeaf()) {
							loc = l;
							return false;
						}
						else {
							nodeId = currentNode->getChild(txn, l);
						}
					}
					else if (cmpResult > 0) {
						if (currentNode->isLeaf()) {
							loc = l + 1;
							return false;
						}
						else {
							nodeId = currentNode->getChild(txn, l + 1);
						}
					}
					else {
						if (currentNode->isRoot()) {
							getRootBNodeObject<K, V>(node);
						}
						loc = l;
						return true;
					}
				}
				if (nodeId != UNDEF_OID) {
					node.load(nodeId);
					currentNode = &node;
				}
				else {
					break;
				}
			}
			assert(0);
		}
		node.reset();  
		loc = 0;
		return false;
	}

	template <typename K, typename V>
	void findUpNode(
		TransactionContext &txn, const BNode<K, V> &node, int32_t &loc) {
		loc = -1;
		BNode<K, V> baseParentNode(
			txn, *getObjectManager(), node.getParentOId(), allocateStrategy_);
		for (int32_t i = 0; i <= baseParentNode.numkeyValues(); ++i) {
			if (baseParentNode.getChild(txn, i) == node.getSelfOId()) {
				loc = i;
				return;
			}
		}
	}
	template <typename K, typename V>
	void splitNode(TransactionContext &txn, BNode<K, V> &dirtyNode1,
		BNode<K, V> &dirtyNode2, KeyValue<K, V> &val) {
		val = dirtyNode1.getKeyValue(nodeMinSize_);
		OId node2Id;
		dirtyNode2.allocateNeighbor<BNodeImage<K, V> >(
			getNormalNodeSize<K, V>(), allocateStrategy_, node2Id,
			dirtyNode1.getSelfOId(), OBJECT_TYPE_BTREE_MAP);

		dirtyNode2.initialize(
			txn, node2Id, dirtyNode1.isLeaf(), nodeMaxSize_, nodeBlockType_);
		for (int32_t i = nodeMinSize_ + 1, j = 0; i < nodeMaxSize_; i++, j++) {
			dirtyNode2.setKeyValue(j, dirtyNode1.getKeyValue(i));
			dirtyNode2.addNumkeyValues(1);
		}
		dirtyNode2.setParentOId(dirtyNode1.getParentOId());
		if (!dirtyNode1.isLeaf()) {
			for (int32_t i = 0; i < nodeMinSize_ + 1; ++i) {
				OId srcOId = dirtyNode1.getChild(txn, nodeMinSize_ + 1 + i);
				dirtyNode2.setChild(txn, i, srcOId);
				dirtyNode1.setChild(txn, nodeMinSize_ + 1 + i, UNDEF_OID);
				if (srcOId != UNDEF_OID) {
					BNode<K, V> dirtyChildNode(
						txn, *getObjectManager(), allocateStrategy_);
					dirtyChildNode.load(srcOId, true);
					dirtyChildNode.setParentOId(node2Id);
				}
			}
		}
		dirtyNode1.setNumkeyValues(nodeMinSize_);
	}

	template <typename K, typename V>
	bool prevPos(TransactionContext &txn, BNode<K, V> &node, int32_t &loc) {
		BNode<K, V> tmpNode(txn, *getObjectManager(), allocateStrategy_);
		tmpNode.copyReference(node);
		int32_t tmpLoc = loc;
		if (node.isLeaf()) {
			if (loc > 0) {
				loc--;
				return true;
			}
			else {
				while (!node.isRoot()) {
					findUpNode(txn, node, loc);
					node.load(node.getParentOId());
					if (loc > 0) {
						break;
					}
				}
				if (loc > 0) {
					loc--;
					return true;
				}
			}
		}
		else {
			if (loc >= 0) {
				node.load(node.getChild(txn, loc));
				while (!node.isLeaf()) {
					node.load(node.getChild(txn, node.numkeyValues()));
				}
				loc = node.numkeyValues() - 1;
				return true;
			}
			else {
				while (!node.isRoot()) {
					findUpNode(txn, node, loc);
					node.load(node.getParentOId());
					if (loc > 0) {
						break;
					}
				}
				if (loc > 0) {
					loc--;
					return true;
				}
			}
		}
		node.copyReference(tmpNode);  
		loc = tmpLoc;
		return false;
	}

	template <typename K, typename V>
	bool nextPos(TransactionContext &txn, BNode<K, V> &node, int32_t &loc) {
		BNode<K, V> tmpNode(txn, *getObjectManager(), allocateStrategy_);
		tmpNode.copyReference(node);
		int32_t tmpLoc = loc;
		if (node.isLeaf()) {
			if (node.numkeyValues() - 1 > loc) {
				loc++;
				return true;
			}
			if (!tmpNode.isRoot()) {
				while (!node.isRoot()) {
					findUpNode(txn, node, loc);
					node.load(node.getParentOId());
					if (node.numkeyValues() > loc) {
						break;
					}
				}
				if (node.numkeyValues() > loc) {
					return true;
				}
			}
		}
		else {
			loc++;
			if (node.numkeyValues() >= loc) {
				node.load(node.getChild(txn, loc));
				while (!node.isLeaf()) {
					node.load(node.getChild(txn, 0));
				}
				loc = 0;
				return true;
			}
			else {
				while (!node.isRoot()) {
					findUpNode(txn, node, loc);
					node.load(node.getParentOId());
					if (node.numkeyValues() >= loc) {
						break;
					}
				}
				if (node.numkeyValues() >= loc) {
					return true;
				}
			}
		}
		node.copyReference(tmpNode);  
		loc = tmpLoc;
		return false;
	}

private:
	template <typename P, typename K, typename V>
	bool insertInternal(
		TransactionContext &txn, P key, V value,
		int32_t (*cmpInput)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<P, V> &e1,
			const KeyValue<K, V> &e2),
		int32_t (*cmpInternal)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<K, V> &e1,
			const KeyValue<K, V> &e2));
	template <typename P, typename K, typename V>
	bool removeInternal(
		TransactionContext &txn, P key, V value,
		int32_t (*cmpInput)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<P, V> &e1,
			const KeyValue<K, V> &e2),
		int32_t (*cmpInternal)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<K, V> &e1,
			const KeyValue<K, V> &e2));
	template <typename P, typename K, typename V>
	bool updateInternal(
		TransactionContext &txn, P key, V value, V newValue,
		int32_t (*cmpInput)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<P, V> &e1,
			const KeyValue<K, V> &e2));

	template <typename P, typename K, typename V, typename R>
	bool findLess(TransactionContext &txn, P key, int32_t isIncluded,
		ResultSize limit,
		int32_t (*keyCmpRight)(TransactionContext &txn,
					  ObjectManager &objectManager, const KeyValue<K, V> &e1,
					  const KeyValue<P, V> &e2),
		util::XArray<R> &result);
	template <typename P, typename K, typename V, typename R>
	bool findLessByDescending(
		TransactionContext &txn, P key, int32_t isIncluded, ResultSize limit,
		int32_t (*keyCmpLeft)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<P, V> &e1,
			const KeyValue<K, V> &e2),
		int32_t (*keyCmpRight)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<K, V> &e1,
			const KeyValue<P, V> &e2),
		util::XArray<R> &result);
	template <typename P, typename K, typename V, typename R>
	bool findGreater(TransactionContext &txn, P key, int32_t isIncluded,
		ResultSize limit,
		int32_t (*keyCmpLeft)(TransactionContext &txn,
						 ObjectManager &objectManager, const KeyValue<P, V> &e1,
						 const KeyValue<K, V> &e2),
		util::XArray<R> &result);
	template <typename P, typename K, typename V, typename R>
	bool findGreaterByDescending(
		TransactionContext &txn, P key, int32_t isIncluded, ResultSize limit,
		int32_t (*keyCmpLeft)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<P, V> &e1,
			const KeyValue<K, V> &e2),
		util::XArray<R> &result);
	template <typename P, typename K, typename V, typename R>
	bool findRange(TransactionContext &txn, P startKey, int32_t isStartIncluded,
		P endKey, int32_t isEndIncluded, ResultSize limit,
		int32_t (*keyCmpLeft)(TransactionContext &txn,
					   ObjectManager &objectManager, const KeyValue<P, V> &e1,
					   const KeyValue<K, V> &e2),
		int32_t (*keyCmpRight)(TransactionContext &txn,
					   ObjectManager &objectManager, const KeyValue<K, V> &e1,
					   const KeyValue<P, V> &e2),
		util::XArray<R> &result);
	template <typename P, typename K, typename V, typename R>
	bool findRangeByDescending(
		TransactionContext &txn, P startKey, int32_t isStartIncluded, P endKey,
		int32_t isEndIncluded, ResultSize limit,
		int32_t (*keyCmpLeft)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<P, V> &e1,
			const KeyValue<K, V> &e2),
		int32_t (*keyCmpRight)(TransactionContext &txn,
			ObjectManager &objectManager, const KeyValue<K, V> &e1,
			const KeyValue<P, V> &e2),
		util::XArray<R> &result);

	template <typename K, typename V>
	void split(TransactionContext &txn, BNode<K, V> &node,
		int32_t (*cmpInternal)(TransactionContext &txn,
				   ObjectManager &objectManager, const KeyValue<K, V> &e1,
				   const KeyValue<K, V> &e2));
	template <typename K, typename V>
	void merge(TransactionContext &txn, BNode<K, V> &node,
		int32_t (*cmpInternal)(TransactionContext &txn,
				   ObjectManager &objectManager, const KeyValue<K, V> &e1,
				   const KeyValue<K, V> &e2));

private:
	template <typename P, typename K, typename V>
	int32_t insertInternal(TransactionContext &txn, P key, V value) {
		bool ret;
		ret = insertInternal<P, K, V>(txn, key, value, &valueCmp, &valueCmp);
		return (ret) ? GS_SUCCESS : GS_FAIL;
	}

	template <typename P, typename K, typename V>
	int32_t removeInternal(TransactionContext &txn, P key, V value) {
		bool ret;
		ret = removeInternal<P, K, V>(txn, key, value, &valueCmp, &valueCmp);
		return (ret) ? GS_SUCCESS : GS_FAIL;
	}

	template <typename P, typename K, typename V>
	int32_t updateInternal(
		TransactionContext &txn, P key, V value, V newValue) {
		bool ret;
		if (isUnique()) {
			ret = updateInternal<P, K, V>(txn, key, value, newValue, &valueCmp);
		}
		else {
			ret =
				removeInternal<P, K, V>(txn, key, value, &valueCmp, &valueCmp);
			if (ret) {
				ret = insertInternal<P, K, V>(
					txn, key, newValue, &valueCmp, &valueCmp);
			}
		}
		return (ret) ? GS_SUCCESS : GS_FAIL;

		return ret;
	}

	template <typename K, typename V>
	void finalizeInternal(TransactionContext &txn) {
		OId rootOId = getRootOId();
		if (rootOId != UNDEF_OID) {
			BNode<K, V> dirtyRootNode(this, allocateStrategy_);
			dirtyRootNode.finalize(txn);
			BaseObject::finalize();
		}
	}
	template <typename P, typename K, typename V>
	int32_t find(TransactionContext &txn, P key, V &value) {
		KeyValue<K, V> keyValue;
		int32_t ret = find<P, K, V>(txn, key, keyValue);
		value = keyValue.value_;
		return ret;
	}

	template <typename P, typename K, typename V>
	int32_t find(TransactionContext &txn, P key, KeyValue<K, V> &keyValue) {
		KeyValue<P, V> val;
		val.key_ = key;
		val.value_ = V();
		BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
		int32_t loc;
		bool ret = findNode<P, K, V>(txn, val, node, loc, &keyCmp);
		if (ret) {
			keyValue = node.getKeyValue(loc);
			return GS_SUCCESS;
		}
		else {
			if (getBtreeMapType() == TYPE_UNIQUE_RANGE_KEY) {
				if (prevPos(txn, node, loc)) {
					keyValue = node.getKeyValue(loc);
					return GS_SUCCESS;
				}
				else {
					return GS_FAIL;
				}
			}
			else {
				return GS_FAIL;
			}
		}
	}

	template <typename P, typename K, typename V, typename R>
	int32_t find(TransactionContext &txn, SearchContext &sc,
		util::XArray<R> &idList, OutputOrder outputOrder) {
		if (sc.isEqual_ && isUnique()) {
			if (sc.startKey_ == NULL) {
				GS_THROW_USER_ERROR(
					GS_ERROR_DS_UNEXPECTED_ERROR, "internal error");
			}
			P key = *reinterpret_cast<const P *>(sc.startKey_);
			KeyValue<K, V> keyValue;
			int32_t ret = find<P, K, V>(txn, key, keyValue);
			if (ret == GS_SUCCESS) {
				pushResultList<K, V, R>(keyValue, idList);
			}
		}
		else if (sc.startKey_ != NULL) {
			P startKey = *reinterpret_cast<const P *>(sc.startKey_);
			if (sc.endKey_ != NULL) {
				P endKey = *reinterpret_cast<const P *>(sc.endKey_);
				if (outputOrder != ORDER_DESCENDING) {
					findRange<P, K, V, R>(txn, startKey, sc.isStartKeyIncluded_,
						endKey, sc.isEndKeyIncluded_, sc.limit_, &keyCmp,
						&keyCmp, idList);
				}
				else {
					findRangeByDescending<P, K, V, R>(txn, startKey,
						sc.isStartKeyIncluded_, endKey, sc.isEndKeyIncluded_,
						sc.limit_, &keyCmp, &keyCmp, idList);
				}
			}
			else {
				if (outputOrder != ORDER_DESCENDING) {
					findGreater<P, K, V, R>(txn, startKey,
						sc.isStartKeyIncluded_, sc.limit_, &keyCmp, idList);
				}
				else {
					findGreaterByDescending<P, K, V, R>(txn, startKey,
						sc.isStartKeyIncluded_, sc.limit_, &keyCmp, idList);
				}
			}
		}
		else if (sc.endKey_ != NULL) {
			P endKey = *reinterpret_cast<const P *>(sc.endKey_);
			if (outputOrder != ORDER_DESCENDING) {
				findLess<P, K, V, R>(txn, endKey, sc.isEndKeyIncluded_,
					sc.limit_, &keyCmp, idList);
			}
			else {
				findLessByDescending<P, K, V, R>(txn, endKey,
					sc.isEndKeyIncluded_, sc.limit_, &keyCmp, &keyCmp, idList);
			}
		}
		else {
			if (outputOrder != ORDER_DESCENDING) {
				getAllByAscending<K, V, R>(txn, sc.limit_, idList);
			}
			else {
				getAllByDescending<K, V, R>(txn, sc.limit_, idList);
			}
		}
		if (idList.empty()) {
			return GS_FAIL;
		}
		else {
			return GS_SUCCESS;
		}
	}

	template <typename K, typename V, typename R>
	int32_t getAllByAscending(
		TransactionContext &txn, ResultSize limit, util::XArray<R> &result) {
		if (isEmpty()) {
			return GS_SUCCESS;
		}
		BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
		getHeadNode<K, V>(txn, node);
		int32_t loc = 0;
		while (true) {
			pushResultList<K, V, R>(node.getKeyValue(loc), result);
			if (!nextPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
		return GS_SUCCESS;
	}

	template <typename K, typename V>
	int32_t getAllByAscending(TransactionContext &txn, ResultSize limit,
		util::XArray<V> &result, BtreeMap::BtreeCursor &cursor) {
		if (isEmpty()) {
			return GS_SUCCESS;
		}
		BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
		int32_t loc = 0;
		if (cursor.nodeId_ == UNDEF_OID) {
			getHeadNode<K, V>(txn, node);
			loc = 0;
		}
		else {
			node.load(cursor.nodeId_);
			loc = cursor.loc_;
		}
		bool hasNext = false;
		while (true) {
			result.push_back(node.getKeyValue(loc).value_);
			hasNext = nextPos(txn, node, loc);

			if (!hasNext || result.size() >= limit) {
				break;
			}
		}
		if (hasNext) {
			cursor.nodeId_ = node.getSelfOId();
			cursor.loc_ = loc;
			return GS_FAIL;
		}
		else {
			return GS_SUCCESS;
		}
	}

	template <typename K, typename V, typename R>
	int32_t getAllByDescending(
		TransactionContext &txn, ResultSize limit, util::XArray<R> &result) {
		if (isEmpty()) {
			return GS_SUCCESS;
		}
		BNode<K, V> node(
			txn, *getObjectManager(), getTailNodeOId(), allocateStrategy_);
		int32_t loc = node.numkeyValues() - 1;
		while (true) {
			pushResultList<K, V, R>(node.getKeyValue(loc), result);
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
		return GS_SUCCESS;
	}
private:
	template <typename K, typename V, typename R>
	void pushResultList(
		const KeyValue<K, V> &keyValue, util::XArray<V> &idList) {
		idList.push_back(keyValue.value_);
	}
	template <typename K, typename V, typename R>
	void pushResultList(
		const KeyValue<K, V> &keyValue, util::XArray<KeyValue<K, V> > &idList) {
		idList.push_back(keyValue);
	}

	template <typename K, typename V>
	bool isChangeNodeSize() {
		BNode<K, V> rootNode(this, allocateStrategy_);
		if (rootNode.isLeaf() &&
			rootNode.numkeyValues() == getInitialItemSizeThreshold<K, V>()) {
			return true;
		}
		else {
			return false;
		}
	}
	template <typename K, typename V>
	bool isRedundantRootNode() {
		BNode<K, V> rootNode(this, allocateStrategy_);
		if (rootNode.numkeyValues() == 0 && !rootNode.isLeaf()) {
			return true;
		}
		else {
			return false;
		}
	}
	template <typename K, typename V>
	int32_t depth(TransactionContext &txn) {
		int32_t d = 0;
		for (OId nodeId = getRootOId(); nodeId != UNDEF_OID;) {
			BNode<K, V> node(txn.getPartitionId(), nodeId);
			if (node.isLeaf()) {
				break;
			}
			nodeId = getChildNode(txn, node.getNode(), 0);
			++d;
		}
		return d;
	}
	template <typename K, typename V>
	friend std::ostream &operator<<(std::ostream &out, BtreeMap &foo) {
		foo.print<K, V>(out, foo.getRootOId());
		return out;
	}
	template <typename K, typename V>
	void getTreeStatus(TransactionContext &txn, OId nodeId, int32_t &nodeNum,
		int32_t &realKeyNum, int32_t &keySpaceNum, int32_t &maxDepth,
		int32_t &minDepth, int32_t &maxKeyNode, int32_t &minKeyNode,
		int32_t depth);

	template <typename K, typename V>
	int32_t print(TransactionContext &txn, std::ostream &out, OId nodeId,
		int32_t depth = 1) {
		int32_t totalSize = 0;
		if (nodeId != UNDEF_OID) {
			BNode<K, V> node(
				txn, *getObjectManager(), nodeId, allocateStrategy_);
			int32_t i;
			int32_t size = node.numkeyValues();
			if (depth >= 1) {
				out << std::setw((depth)*5) << "|-->[ " << size << "] ";
			}
			out << node.dump(txn);
			out << "\n";
			if (!node.isLeaf()) {
				for (i = size; i >= 0; --i) {
					totalSize +=
						print<K, V>(txn, out, node.getChild(txn, i), depth + 1);
				}
			}
			totalSize += size;
		}
		return totalSize;
	}

	template <typename K, typename V>
	std::string validateInternal(TransactionContext &txn);
	inline bool isUnique() const {
		BNode<char, char> rootNode(this, allocateStrategy_);
		return rootNode.isUnique() != 0;
	}
	inline ColumnType getKeyType() const {
		BNode<char, char> rootNode(this, allocateStrategy_);
		return rootNode.getKeyType();
	}

	void setTailNodeOId(TransactionContext &, OId oId) {
		BNode<char, char> rootNode(this, allocateStrategy_);
		rootNode.setTailNodeOId(oId);
	}
	OId getRootOId() const {
		return getBaseOId();
	}
	void setRootOId(OId oId) {
		setBaseOId(oId);
	}

	template <typename K, typename V>
	void replaceRoot(TransactionContext &txn, BNode<K, V> &replaceNode) {
		BNode<K, V> baseRootNode(this, allocateStrategy_);

		getObjectManager()->setDirty(
			txn.getPartitionId(), replaceNode.getSelfOId());
		replaceNode.setRootNodeHeader(baseRootNode.getKeyType(),
			baseRootNode.getBtreeMapType(), baseRootNode.isUnique(),
			baseRootNode.getTailNodeOId());

		this->copyReference(replaceNode);
	}

	template <typename K, typename V>
	void getRootBNodeObject(BNode<K, V> &rootNode) const {
		rootNode.directLoad(this);
	}
};

template <typename K, typename V>
void BtreeMap::split(TransactionContext &txn, BNode<K, V> &dirtyNode1,
	int32_t (*cmpInternal)(TransactionContext &txn,
						 ObjectManager &objectManager, const KeyValue<K, V> &e1,
						 const KeyValue<K, V> &e2)) {
	if (dirtyNode1.numkeyValues() == 2 * nodeMinSize_ + 1) {
		int32_t i, j;
		OId parentOId;
		KeyValue<K, V> middleVal;
		BNode<K, V> dirtyNode2(txn, *getObjectManager(), allocateStrategy_);
		splitNode(txn, dirtyNode1, dirtyNode2, middleVal);

		BNode<K, V> dirtyParentNode(
			txn, *getObjectManager(), allocateStrategy_);
		if (dirtyNode1.isRoot()) {
			dirtyParentNode.allocateNeighbor<BNodeImage<K, V> >(
				getNormalNodeSize<K, V>(), allocateStrategy_, parentOId,
				dirtyNode1.getSelfOId(), OBJECT_TYPE_BTREE_MAP);
			dirtyParentNode.initialize(
				txn, parentOId, false, nodeMaxSize_, nodeBlockType_);
			dirtyParentNode.setKeyValue(0, middleVal);
			dirtyParentNode.setNumkeyValues(1);
			dirtyParentNode.setChild(txn, 0, dirtyNode1.getSelfOId());
			dirtyParentNode.setChild(txn, 1, dirtyNode2.getSelfOId());
			dirtyNode1.setParentOId(dirtyParentNode.getSelfOId());
			dirtyNode2.setParentOId(dirtyParentNode.getSelfOId());
			replaceRoot<K, V>(txn, dirtyParentNode);
		}
		else {
			dirtyParentNode.load(dirtyNode1.getParentOId(), OBJECT_FOR_UPDATE);
			int32_t parentNodeSize = dirtyParentNode.numkeyValues();
			if (cmpInternal(txn, *getObjectManager(), middleVal,
					dirtyParentNode.getKeyValue(parentNodeSize - 1)) > 0) {
				dirtyParentNode.setChild(
					txn, parentNodeSize + 1, dirtyNode2.getSelfOId());
				dirtyParentNode.setKeyValue(parentNodeSize, middleVal);
				dirtyParentNode.addNumkeyValues(1);
			}
			else {
				for (i = 0; i < parentNodeSize; ++i) {
					if (cmpInternal(txn, *getObjectManager(), middleVal,
							dirtyParentNode.getKeyValue(i)) < 0) {
						for (j = parentNodeSize; j > i; --j) {
							dirtyParentNode.setKeyValue(
								j, dirtyParentNode.getKeyValue(j - 1));
						}
						dirtyParentNode.setKeyValue(i, middleVal);
						dirtyParentNode.addNumkeyValues(1);
						for (j = parentNodeSize + 1; j > i + 1; --j) {
							dirtyParentNode.setChild(
								txn, j, dirtyParentNode.getChild(txn, j - 1));
						}
						dirtyParentNode.setChild(
							txn, i + 1, dirtyNode2.getSelfOId());
						break;
					}
				}
			}
		}
		if (dirtyNode1.getSelfOId() == getTailNodeOId()) {
			setTailNodeOId(txn, dirtyNode2.getSelfOId());
		}

		split<K, V>(txn, dirtyParentNode, cmpInternal);
	}
}

template <typename K, typename V>
void BtreeMap::merge(TransactionContext &txn, BNode<K, V> &node,
	int32_t (*cmpInternal)(TransactionContext &txn,
						 ObjectManager &objectManager, const KeyValue<K, V> &e1,
						 const KeyValue<K, V> &e2)) {
	if (node.numkeyValues() < nodeMinSize_ && !node.isRoot()) {
		int32_t upIndex;
		findUpNode(txn, node, upIndex);
		BNode<K, V> parentNode(txn, *getObjectManager(), node.getParentOId(),
			allocateStrategy_);  
		BNode<K, V> preSibNode(txn, *getObjectManager(),
			allocateStrategy_);  

		if (upIndex > 0) {
			preSibNode.load(parentNode.getChild(txn, upIndex - 1));
		}
		if (preSibNode.getBaseOId() != UNDEF_OID &&
			preSibNode.numkeyValues() > nodeMinSize_) {
			getObjectManager()->setDirty(
				txn.getPartitionId(), node.getSelfOId());
			getObjectManager()->setDirty(
				txn.getPartitionId(), preSibNode.getSelfOId());
			getObjectManager()->setDirty(
				txn.getPartitionId(), parentNode.getSelfOId());
			BNode<K, V> *dirtyNode = &node;
			BNode<K, V> *dirtyPreSibNode = &preSibNode;
			BNode<K, V> *dirtyParentNode = &parentNode;
			int32_t sibNodeSize = dirtyPreSibNode->numkeyValues();

			if (!dirtyNode->isLeaf()) {
				BNode<K, V> dirtyChildNode(
					txn, *getObjectManager(), allocateStrategy_);
				dirtyChildNode.load(
					dirtyPreSibNode->getChild(txn, sibNodeSize), true);
				dirtyNode->insertChild(txn, 0, dirtyChildNode);
			}
			dirtyNode->insertVal(
				txn, dirtyParentNode->getKeyValue(upIndex - 1), cmpInternal);

			dirtyParentNode->setKeyValue(
				upIndex - 1, dirtyPreSibNode->getKeyValue(sibNodeSize - 1));

			if (!dirtyNode->isLeaf()) {
				dirtyPreSibNode->removeChild(txn, sibNodeSize);
			}
			dirtyPreSibNode->removeVal(sibNodeSize - 1);
		}
		else {
			BNode<K, V> followSibNode(txn, *getObjectManager(),
				allocateStrategy_);  
			if (upIndex < parentNode.numkeyValues()) {
				followSibNode.load(parentNode.getChild(txn, upIndex + 1));
			}
			if (followSibNode.getBaseOId() != UNDEF_OID &&
				followSibNode.numkeyValues() > nodeMinSize_) {
				getObjectManager()->setDirty(
					txn.getPartitionId(), node.getSelfOId());
				getObjectManager()->setDirty(
					txn.getPartitionId(), followSibNode.getSelfOId());
				getObjectManager()->setDirty(
					txn.getPartitionId(), parentNode.getSelfOId());
				BNode<K, V> *dirtyNode = &node;
				BNode<K, V> *dirtyFollowSibNode = &followSibNode;
				BNode<K, V> *dirtyParentNode = &parentNode;
				int32_t nodeObjectSize = dirtyNode->numkeyValues();

				if (!dirtyNode->isLeaf()) {
					BNode<K, V> dirtyChildNode(
						txn, *getObjectManager(), allocateStrategy_);
					dirtyChildNode.load(
						dirtyFollowSibNode->getChild(txn, 0), true);
					dirtyNode->insertChild(
						txn, nodeObjectSize + 1, dirtyChildNode);
				}
				dirtyNode->insertVal(
					txn, dirtyParentNode->getKeyValue(upIndex), cmpInternal);

				dirtyParentNode->setKeyValue(
					upIndex, dirtyFollowSibNode->getKeyValue(0));
				if (!dirtyNode->isLeaf()) {
					dirtyFollowSibNode->removeChild(txn, 0);
				}
				dirtyFollowSibNode->removeVal(0);

			}
			else if (preSibNode.getBaseOId() != UNDEF_OID &&
					 preSibNode.numkeyValues() == nodeMinSize_) {
				getObjectManager()->setDirty(
					txn.getPartitionId(), node.getSelfOId());
				getObjectManager()->setDirty(
					txn.getPartitionId(), preSibNode.getSelfOId());
				getObjectManager()->setDirty(
					txn.getPartitionId(), parentNode.getSelfOId());
				BNode<K, V> *dirtyNode = &node;
				BNode<K, V> *dirtyPreSibNode = &preSibNode;
				BNode<K, V> *dirtyParentNode = &parentNode;
				int32_t sibNodeSize = preSibNode.numkeyValues();

				dirtyPreSibNode->insertVal(txn,
					dirtyParentNode->getKeyValue(upIndex - 1), cmpInternal);

				for (int32_t i = 0; i < dirtyNode->numkeyValues(); ++i) {
					dirtyPreSibNode->setKeyValue(
						sibNodeSize + i + 1, dirtyNode->getKeyValue(i));
				}
				dirtyPreSibNode->addNumkeyValues(dirtyNode->numkeyValues());

				dirtyParentNode->removeChild(txn, upIndex);
				dirtyParentNode->removeVal(upIndex - 1);

				if (!dirtyNode->isLeaf()) {
					for (int32_t i = 0; i <= dirtyNode->numkeyValues(); ++i) {
						OId srcOId = dirtyNode->getChild(txn, i);
						dirtyPreSibNode->setChild(
							txn, sibNodeSize + i + 1, srcOId);
						if (srcOId != UNDEF_OID) {
							BNode<K, V> dirtyChildNode(
								txn, *getObjectManager(), allocateStrategy_);
							dirtyChildNode.load(srcOId, true);
							dirtyChildNode.setParentOId(
								dirtyPreSibNode->getSelfOId());
						}
					}
				}
				if (dirtyNode->getSelfOId() == getTailNodeOId()) {
					setTailNodeOId(txn, dirtyPreSibNode->getSelfOId());
				}
				dirtyNode->remove(txn);
			}
			else if (followSibNode.getBaseOId() != UNDEF_OID &&
					 followSibNode.numkeyValues() == nodeMinSize_) {
				getObjectManager()->setDirty(
					txn.getPartitionId(), node.getSelfOId());
				getObjectManager()->setDirty(
					txn.getPartitionId(), followSibNode.getSelfOId());
				getObjectManager()->setDirty(
					txn.getPartitionId(), parentNode.getSelfOId());
				BNode<K, V> *dirtyNode = &node;
				BNode<K, V> *dirtyFollowSibNode = &followSibNode;
				BNode<K, V> *dirtyParentNode = &parentNode;
				int32_t nodeObjectSize = dirtyNode->numkeyValues();
				int32_t sibNodeSize = dirtyFollowSibNode->numkeyValues();

				dirtyNode->insertVal(
					txn, dirtyParentNode->getKeyValue(upIndex), cmpInternal);

				for (int32_t i = 0; i < sibNodeSize; ++i) {
					dirtyNode->setKeyValue(nodeObjectSize + i + 1,
						dirtyFollowSibNode->getKeyValue(i));
				}
				dirtyNode->addNumkeyValues(sibNodeSize);

				dirtyParentNode->removeChild(txn, upIndex + 1);
				dirtyParentNode->removeVal(upIndex);

				if (!dirtyNode->isLeaf()) {
					for (int32_t i = 0; i <= sibNodeSize; ++i) {
						OId srcOId = dirtyFollowSibNode->getChild(txn, i);
						dirtyNode->setChild(
							txn, nodeObjectSize + i + 1, srcOId);
						if (srcOId != UNDEF_OID) {
							BNode<K, V> dirtyChildNode(
								txn, *getObjectManager(), allocateStrategy_);
							dirtyChildNode.load(srcOId, true);
							dirtyChildNode.setParentOId(
								dirtyNode->getSelfOId());
						}
					}
				}
				if (dirtyFollowSibNode->getSelfOId() == getTailNodeOId()) {
					setTailNodeOId(txn, dirtyNode->getSelfOId());
				}
				dirtyFollowSibNode->remove(txn);
			}
		}
		merge(txn, parentNode, cmpInternal);
	}
}

template <typename P, typename K, typename V>
bool BtreeMap::insertInternal(
	TransactionContext &txn, P key, V value,
	int32_t (*cmpInput)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	int32_t (*cmpInternal)(TransactionContext &txn,
		ObjectManager &objectManager, const KeyValue<K, V> &e1,
		const KeyValue<K, V> &e2)) {
	KeyValue<P, V> val;
	val.key_ = key;
	val.value_ = value;
	if (isEmpty()) {
		BNode<K, V> dirtyNode(txn, *getObjectManager(), allocateStrategy_);
		getRootBNodeObject<K, V>(dirtyNode);
		dirtyNode.allocateVal(txn, 0, val);
		setTailNodeOId(txn, getRootOId());
	}
	else {
		if (isChangeNodeSize<K, V>()) {
			BNode<K, V> rootNode(
				txn, *getObjectManager(), getRootOId(), allocateStrategy_);
			BNode<K, V> dirtyNewNode(
				txn, *getObjectManager(), allocateStrategy_);
			OId newNodeOId;
			dirtyNewNode.allocate<BNodeImage<K, V> >(getNormalNodeSize<K, V>(),
				allocateStrategy_, newNodeOId, OBJECT_TYPE_BTREE_MAP);
			dirtyNewNode.initialize(
				txn, newNodeOId, true, nodeMaxSize_, nodeBlockType_);

			for (int32_t i = 0; i < rootNode.numkeyValues(); ++i) {
				dirtyNewNode.setKeyValue(i, rootNode.getKeyValue(i));
			}
			dirtyNewNode.setNumkeyValues(rootNode.numkeyValues());

			replaceRoot<K, V>(txn, dirtyNewNode);
			setTailNodeOId(txn, getRootOId());
			rootNode.remove(txn);
		}
		{
			int32_t loc;
			BNode<K, V> dirtyNode1(txn, *getObjectManager(),
				allocateStrategy_);  
			{
				dirtyNode1.load(getTailNodeOId());
				if (cmpInput(txn, *getObjectManager(), val,
						dirtyNode1.getKeyValue(dirtyNode1.numkeyValues() - 1)) >
					0) {
					loc = dirtyNode1.numkeyValues();
				}
				else {
					if (findNode<P, K, V>(
							txn, val, dirtyNode1, loc, cmpInput) &&
						isUnique()) {
						return false;
					}
				}
			}

			getObjectManager()->setDirty(
				txn.getPartitionId(), dirtyNode1.getSelfOId());
			for (int32_t i = dirtyNode1.numkeyValues(); i > loc; --i) {
				dirtyNode1.setKeyValue(i, dirtyNode1.getKeyValue(i - 1));
			}

			dirtyNode1.allocateVal(txn, loc, val);

			split<K, V>(txn, dirtyNode1, cmpInternal);
		}
	}
	return true;
}

template <typename P, typename K, typename V>
bool BtreeMap::removeInternal(
	TransactionContext &txn, P key, V value,
	int32_t (*cmpInput)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	int32_t (*cmpInternal)(TransactionContext &txn,
		ObjectManager &objectManager, const KeyValue<K, V> &e1,
		const KeyValue<K, V> &e2)) {
	{
		KeyValue<P, V> val;
		val.key_ = key;
		val.value_ = value;
		int32_t loc;
		BNode<K, V> dirtyUpdateNode(txn, *getObjectManager(),
			allocateStrategy_);  

		if (!findNode<P, K, V>(txn, val, dirtyUpdateNode, loc, cmpInput)) {
			return false;
		}

		getObjectManager()->setDirty(
			txn.getPartitionId(), dirtyUpdateNode.getSelfOId());

		if (!dirtyUpdateNode.isLeaf()) {
			BNode<K, V> dirtyUpdateChildNode(txn, *getObjectManager(),
				dirtyUpdateNode.getChild(txn, loc),
				allocateStrategy_);  

			while (!dirtyUpdateChildNode.isLeaf()) {
				dirtyUpdateChildNode.load(dirtyUpdateChildNode.getChild(
					txn, dirtyUpdateChildNode.numkeyValues()));
			}
			getObjectManager()->setDirty(
				txn.getPartitionId(), dirtyUpdateChildNode.getSelfOId());

			dirtyUpdateNode.freeVal(txn, loc);
			dirtyUpdateNode.setKeyValue(loc,
				dirtyUpdateChildNode.getKeyValue(
					dirtyUpdateChildNode.numkeyValues() - 1));  
			loc = dirtyUpdateChildNode.numkeyValues() - 1;

			dirtyUpdateNode.copyReference(dirtyUpdateChildNode);
		}
		else {
			dirtyUpdateNode.freeVal(txn, loc);
		}
		if (dirtyUpdateNode.isRoot()) {
			dirtyUpdateNode.removeVal(loc);
		}
		else {
			dirtyUpdateNode.removeVal(loc);
			merge(txn, dirtyUpdateNode, cmpInternal);
		}
	}
	{
		if (isRedundantRootNode<K, V>()) {
			BNode<K, V> rootNode(
				txn, *getObjectManager(), getRootOId(), allocateStrategy_);
			OId newrootId = rootNode.getChild(txn, 0);
			BNode<K, V> dirtyNewRootNode(
				txn, *getObjectManager(), allocateStrategy_);
			dirtyNewRootNode.load(newrootId, true);
			dirtyNewRootNode.setParentOId(UNDEF_OID);
			replaceRoot<K, V>(txn, dirtyNewRootNode);
			rootNode.remove(txn);
		}
	}
	{
		if (isChangeNodeSize<K, V>()) {
			BNode<K, V> dirtyNewNode(
				txn, *getObjectManager(), allocateStrategy_);
			OId newNodeOId;
			dirtyNewNode.allocate<BNodeImage<K, V> >(getInitialNodeSize<K, V>(),
				allocateStrategy_, newNodeOId, OBJECT_TYPE_BTREE_MAP);
			dirtyNewNode.initialize(txn, dirtyNewNode.getBaseOId(), true,
				getInitialItemSizeThreshold<K, V>(), nodeBlockType_);

			BNode<K, V> dirtyOldRootNode(
				txn, *getObjectManager(), getRootOId(), allocateStrategy_);
			for (int32_t i = 0; i < dirtyOldRootNode.numkeyValues(); ++i) {
				dirtyNewNode.setKeyValue(i, dirtyOldRootNode.getKeyValue(i));
			}
			dirtyNewNode.setNumkeyValues(dirtyOldRootNode.numkeyValues());

			replaceRoot<K, V>(txn, dirtyNewNode);
			setTailNodeOId(txn, getRootOId());

			dirtyOldRootNode.remove(txn);
		}
	}
	return true;
}

template <typename P, typename K, typename V>
bool BtreeMap::updateInternal(
	TransactionContext &txn, P key, V value, V newValue,
	int32_t (*cmpInput)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2)) {
	KeyValue<P, V> val;
	val.key_ = key;
	val.value_ = value;
	int32_t orgLoc;

	BNode<K, V> dirtyOrgNode(txn, *getObjectManager(),
		allocateStrategy_);  
	if (!findNode<P, K, V>(txn, val, dirtyOrgNode, orgLoc, cmpInput)) {
		return false;
	}
	getObjectManager()->setDirty(
		txn.getPartitionId(), dirtyOrgNode.getSelfOId());

	dirtyOrgNode.setValue(orgLoc, newValue);

	return true;
}


template <typename P, typename K, typename V, typename R>
bool BtreeMap::findLess(TransactionContext &txn, P key, int32_t isIncluded,
	ResultSize limit, int32_t (*keyCmpRight)(TransactionContext &txn,
							ObjectManager &objectManager,
							const KeyValue<K, V> &e1, const KeyValue<P, V> &e2),
	util::XArray<R> &result) {
	KeyValue<P, V> val;
	BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
	int32_t loc;
	val.key_ = key;

	if (isEmpty()) {
		return false;
	}
	else {
		getHeadNode<K, V>(txn, node);
		loc = 0;
	}
	while (true) {
		if (keyCmpRight(txn, *getObjectManager(), node.getKeyValue(loc), val) <
			isIncluded) {
			pushResultList<K, V, R>(node.getKeyValue(loc), result);
		}
		else {
			break;
		}
		if (!nextPos(txn, node, loc) || result.size() >= limit) {
			break;
		}
	}
	return true;
}

template <typename P, typename K, typename V, typename R>
bool BtreeMap::findLessByDescending(
	TransactionContext &txn, P key, int32_t isIncluded, ResultSize limit,
	int32_t (*keyCmpLeft)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	int32_t (*keyCmpRight)(TransactionContext &txn,
		ObjectManager &objectManager, const KeyValue<K, V> &e1,
		const KeyValue<P, V> &e2),
	util::XArray<R> &result) {
	KeyValue<P, V> val;
	BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
	int32_t loc;
	val.key_ = key;
	findNode(txn, val, node, loc, keyCmpLeft);
	if (loc >= node.numkeyValues()) {
		while (true) {
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
			else {
				pushResultList<K, V, R>(node.getKeyValue(loc), result);
			}
		}
		return true;
	}
	else {
		while (isIncluded) {
			if (keyCmpLeft(
					txn, *getObjectManager(), val, node.getKeyValue(loc)) < 0) {
				break;
			}
			if (!nextPos(txn, node, loc)) {
				break;
			}
		}
		bool start = false;
		while (true) {
			if (start) {
				pushResultList<K, V, R>(node.getKeyValue(loc), result);
			}
			else {
				if (keyCmpRight(txn, *getObjectManager(), node.getKeyValue(loc),
						val) < isIncluded) {
					pushResultList<K, V, R>(node.getKeyValue(loc), result);
					start = true;
				}
			}
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
		return true;
	}
}

template <typename P, typename K, typename V, typename R>
bool BtreeMap::findGreater(
	TransactionContext &txn, P key, int32_t isIncluded, ResultSize limit,
	int32_t (*keyCmpLeft)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	util::XArray<R> &result) {
	KeyValue<P, V> val;
	BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
	int32_t loc;
	val.key_ = key;
	bool isEqual = findNode(txn, val, node, loc, keyCmpLeft);

	if (getBtreeMapType() == TYPE_UNIQUE_RANGE_KEY) {
		if (loc >= node.numkeyValues() || !isEqual) {
			prevPos(txn, node, loc);
		}
		while (true) {
			pushResultList<K, V, R>(node.getKeyValue(loc), result);
			if (!nextPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
	}
	else {
		if (loc >= node.numkeyValues()) {
			prevPos(txn, node, loc);
		}
		else {
			while (isIncluded) {
				if (keyCmpLeft(txn, *getObjectManager(), val,
						node.getKeyValue(loc)) > 0) {
					break;
				}
				if (!prevPos(txn, node, loc)) {
					break;
				}
			}
		}
		bool start = false;
		while (true) {
			if (start) {
				pushResultList<K, V, R>(node.getKeyValue(loc), result);
			}
			else {
				if (keyCmpLeft(txn, *getObjectManager(), val,
						node.getKeyValue(loc)) < isIncluded) {
					pushResultList<K, V, R>(node.getKeyValue(loc), result);
					start = true;
				}
			}
			if (!nextPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
	}
	return true;
}

template <typename P, typename K, typename V, typename R>
bool BtreeMap::findGreaterByDescending(
	TransactionContext &txn, P key, int32_t isIncluded, ResultSize limit,
	int32_t (*keyCmpLeft)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	util::XArray<R> &result) {
	KeyValue<P, V> val;
	BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
	int32_t loc;
	val.key_ = key;

	if (isEmpty()) {
		return false;
	}
	else {
		node.load(getTailNodeOId());
		loc = node.numkeyValues() - 1;
	}
	if (getBtreeMapType() == TYPE_UNIQUE_RANGE_KEY) {
		int32_t ret, prevRet = -1;
		while (true) {
			ret = keyCmpLeft(
				txn, *getObjectManager(), val, node.getKeyValue(loc));
			if (ret < isIncluded) {
				pushResultList<K, V, R>(node.getKeyValue(loc), result);
				prevRet = ret;
			}
			else {
				if (ret == 0 || prevRet != 0) {
					pushResultList<K, V, R>(node.getKeyValue(loc), result);
				}
				break;
			}
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
	}
	else {
		while (true) {
			if (keyCmpLeft(txn, *getObjectManager(), val,
					node.getKeyValue(loc)) < isIncluded) {
				pushResultList<K, V, R>(node.getKeyValue(loc), result);
			}
			else {
				break;
			}
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
	}
	return true;
}

template <typename P, typename K, typename V, typename R>
bool BtreeMap::findRange(
	TransactionContext &txn, P startKey, int32_t isStartIncluded, P endKey,
	int32_t isEndIncluded, ResultSize limit,
	int32_t (*keyCmpLeft)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	int32_t (*keyCmpRight)(TransactionContext &txn,
		ObjectManager &objectManager, const KeyValue<K, V> &e1,
		const KeyValue<P, V> &e2),
	util::XArray<R> &result) {
	KeyValue<P, V> startVal, endVal;
	BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
	int32_t loc;
	startVal.key_ = startKey;
	endVal.key_ = endKey;
	bool isEqual = findNode<P, K, V>(txn, startVal, node, loc, keyCmpLeft);
	if (getBtreeMapType() == TYPE_UNIQUE_RANGE_KEY) {
		if (loc >= node.numkeyValues() || !isEqual) {
			prevPos(txn, node, loc);
		}
		while (true) {
			if (keyCmpRight(txn, *getObjectManager(), node.getKeyValue(loc),
					endVal) < isEndIncluded) {
				pushResultList<K, V, R>(node.getKeyValue(loc), result);
			}
			else {
				break;
			}

			if (!nextPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
	}
	else {
		if (loc >= node.numkeyValues()) {
			prevPos(txn, node, loc);
		}
		else {
			while (isStartIncluded) {
				if (keyCmpLeft(txn, *getObjectManager(), startVal,
						node.getKeyValue(loc)) > 0) {
					break;
				}
				if (!prevPos(txn, node, loc)) {
					break;
				}
			}
		}
		bool start = false;
		while (true) {
			if (start) {
				if (keyCmpRight(txn, *getObjectManager(), node.getKeyValue(loc),
						endVal) < isEndIncluded) {
					pushResultList<K, V, R>(node.getKeyValue(loc), result);
				}
				else {
					break;
				}
			}
			else {
				if (keyCmpLeft(txn, *getObjectManager(), startVal,
						node.getKeyValue(loc)) < isStartIncluded) {
					if (keyCmpRight(txn, *getObjectManager(),
							node.getKeyValue(loc), endVal) < isEndIncluded) {
						pushResultList<K, V, R>(node.getKeyValue(loc), result);
						start = true;
					}
					else {
						break;
					}
				}
			}

			if (!nextPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
	}
	return true;
}

template <typename P, typename K, typename V, typename R>
bool BtreeMap::findRangeByDescending(
	TransactionContext &txn, P startKey, int32_t isStartIncluded, P endKey,
	int32_t isEndIncluded, ResultSize limit,
	int32_t (*keyCmpLeft)(TransactionContext &txn, ObjectManager &objectManager,
		const KeyValue<P, V> &e1, const KeyValue<K, V> &e2),
	int32_t (*keyCmpRight)(TransactionContext &txn,
		ObjectManager &objectManager, const KeyValue<K, V> &e1,
		const KeyValue<P, V> &e2),
	util::XArray<R> &result) {
	KeyValue<P, V> startVal, endVal;
	BNode<K, V> node(txn, *getObjectManager(), allocateStrategy_);
	int32_t loc;
	startVal.key_ = startKey;
	endVal.key_ = endKey;
	bool isEqual = findNode(txn, endVal, node, loc, keyCmpLeft);

	if (getBtreeMapType() == TYPE_UNIQUE_RANGE_KEY) {
		if (loc >= node.numkeyValues() || (isEqual && !isEndIncluded)) {
			prevPos(txn, node, loc);
		}
		int32_t ret, prevRet = -1;
		bool start = false;
		while (true) {
			if (start) {
				ret = keyCmpLeft(
					txn, *getObjectManager(), startVal, node.getKeyValue(loc));
				if (ret < isStartIncluded) {
					pushResultList<K, V, R>(node.getKeyValue(loc), result);
					prevRet = ret;
				}
				else {
					if (ret == 0 || prevRet != 0) {
						pushResultList<K, V, R>(node.getKeyValue(loc), result);
					}
					break;
				}
			}
			else {
				if (keyCmpRight(txn, *getObjectManager(), node.getKeyValue(loc),
						endVal) < isEndIncluded) {
					ret = keyCmp(txn, *getObjectManager(), startVal,
						node.getKeyValue(loc));
					if (ret < isStartIncluded) {
						pushResultList<K, V, R>(node.getKeyValue(loc), result);
						prevRet = ret;
						start = true;
					}
					else {
						if (ret == 0 || prevRet != 0) {
							pushResultList<K, V, R>(
								node.getKeyValue(loc), result);
						}
						break;
					}
				}
			}
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
		return true;
	}
	else {
		if (loc >= node.numkeyValues()) {
			prevPos(txn, node, loc);
		}
		else {
			while (isEndIncluded) {
				if (keyCmpLeft(txn, *getObjectManager(), endVal,
						node.getKeyValue(loc)) < 0) {
					break;
				}
				if (!nextPos(txn, node, loc)) {
					break;
				}
			}
		}
		bool start = false;
		while (true) {
			if (start) {
				if (keyCmpLeft(txn, *getObjectManager(), startVal,
						node.getKeyValue(loc)) < isStartIncluded) {
					pushResultList<K, V, R>(node.getKeyValue(loc), result);
				}
				else {
					break;
				}
			}
			else {
				if (keyCmpRight(txn, *getObjectManager(), node.getKeyValue(loc),
						endVal) < isEndIncluded) {
					if (keyCmp(txn, *getObjectManager(), startVal,
							node.getKeyValue(loc)) < isStartIncluded) {
						pushResultList<K, V, R>(node.getKeyValue(loc), result);
						start = true;
					}
					else {
						break;
					}
				}
			}
			if (!prevPos(txn, node, loc) || result.size() >= limit) {
				break;
			}
		}
		return true;
	}
}
template <>
inline void BtreeMap::BNode<StringKey, OId>::freeVal(
	TransactionContext &txn, int32_t m) {
	getObjectManager()->free(
		txn.getPartitionId(), getImage()->keyValues_[m].key_.oId_);
}  
template <>
inline std::string BtreeMap::BNode<StringKey, OId>::dump(
	TransactionContext &txn) {
	util::NormalOStringStream out;
	out << "(@@Node@@)";
	for (int32_t i = 0; i < numkeyValues(); ++i) {
		StringCursor obj(txn, *getObjectManager(), getKeyValue(i).key_.oId_);
		util::NormalString tmp(
			reinterpret_cast<const char *>(obj.str()), obj.stringLength());
		out << "[" << tmp << "," << getKeyValue(i).value_ << "], ";
	}
	return out.str();
}

#endif  
