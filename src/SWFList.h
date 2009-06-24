#ifndef F_List_H
#define F_List_H

#include <cstdio>
#define WARNING printf

namespace SWF {

template <class T> class ListItem;

/*
	a simple (likely non-threadsafe) LinkedList
*/

template <class T>
class List {
	public:
		List( bool ownership = true )
				: mOwnership( ownership ) {
				mFirst = mLast = NULL;
			}
		~List() {
				ListItem<T> *item = mFirst, *next;
				if( mOwnership ) {
					while( item ) {
						if( item->data() ) delete item->data();
						next = item->next();
						delete item;
						item = next;
					}
				} else {
					while( item ) {
						next = item->next();
						delete item;
						item = next;
					}
				}
			}
			
		ListItem<T> *append( T *data, void *user_data = NULL ) {
				if( !mLast ) {
					if( !mFirst ) {
						return mFirst = mLast = new ListItem<T>( data, NULL, user_data );
					} else {
						WARNING("Linked List has First but no Last item\n");
						return NULL;
					}
				} else {
					return mLast = new ListItem<T>( data, mLast, user_data );
				}
			}

		ListItem<T> *prepend( T *data, void *user_data = NULL ) {
				if( !mFirst ) {
					return mFirst = mLast = new ListItem<T>( data, NULL, user_data );
				} else {
					ListItem<T> *item = new ListItem<T>( data, NULL, user_data );
					item->mNext = mFirst;
					mFirst->mPrevious = item;
					return mFirst = item;
				}
			}

		ListItem<T> *prepend2( T *data, void *user_data = NULL ) {
				if( !mFirst ) {
					return mFirst = mLast = new ListItem<T>( data, NULL, user_data );
				} else {
					return new ListItem<T>( data, mFirst, user_data );
				}
			}
			
		T* remove( ListItem<T> *item ) {
				item->remove( this );
				T *ret = item->data();
				if( mOwnership && item->data() ) {
					delete item->data();
					ret = NULL;
				}
				delete item;
				return ret;
			}

		void moveToLast( ListItem<T> *item ) {
				if( item == mLast ) return;
				item->remove( this );
				item->mPrevious = mLast;
				if( mLast ) {
					mLast->mNext = item;
				}
				mLast = item;
				if( !mFirst ) mFirst = item;
			}
			
		ListItem<T> *first() { return mFirst; };
		ListItem<T> *last() { return mLast; };

		friend class ListItem<T>;
		
		int size() {
			if (mFirst == NULL) return 0;
			if (mFirst == mLast) return 1;
			
			int count = 1;
			ListItem<T> *item = mFirst;
			while(item != mLast) {
				count++;
				item = item->next();
			}
			
			return count;
		}
			
	protected:
		ListItem<T> *mFirst;
		ListItem<T> *mLast;
		bool mOwnership;
};

template <class T>
class ListItem {
	public:
		ListItem( T *data, ListItem* insertAfter = NULL, void *user_data = NULL ) {
				mData = data;
				mPrevious = insertAfter;
				mNext = insertAfter ? insertAfter->replaceNext( this ) : NULL;
				if( mNext ) mNext->mPrevious = this;
				mUserData = user_data;
			}
		
		ListItem *replaceNext( ListItem *next ) {
				ListItem *ret = mNext;
				mNext = next;
				return ret;
			}
			
		void remove( List<T> *list ) {
				if( !mPrevious ) list->mFirst = mNext;
				else mPrevious->mNext = mNext;
				if( !mNext ) list->mLast = mPrevious;
				else mNext->mPrevious = mPrevious;
				mPrevious = mNext = NULL;
			}
			
		ListItem *next() {
				return( mNext );
			}

		ListItem *previous() {
				return( mPrevious );
			}

		T* data() {
				return( mData );
			}

		void* UserData() {
				return( mUserData );
			}
			
		bool isLast() {
				return( mNext == 0 );
			}
		bool isFirst() {
				return( mPrevious == 0 );
			}

		friend class List<T>;
			
	protected:
		ListItem *mPrevious;
		ListItem *mNext;
		T *mData;
		void *mUserData;
};


}

#endif
