#ifndef ARRAY_AUTO_PTR_H
#define ARRAY_AUTO_PTR_H


template <typename T>
class ArrayAutoPtr {
	public:
		explicit ArrayAutoPtr(T* a=NULL)
			: owner(a), array(a) { }

		ArrayAutoPtr(const ArrayAutoPtr& other)
			: owner(other.owner), array(other.release()) { }

		~ArrayAutoPtr() {
			if (owner) {
				delete[] array;
			}
		}

		ArrayAutoPtr& operator=(const ArrayAutoPtr& other) {
			if ((void*)&other != (void*)this) {
				if (owner) {
					delete[] array;
				}
				owner = other.owner;
				array = other.release();
			}
		}

		ArrayAutoPtr& operator=(T* a) {
			if (array != a) {
				if (owner) {
					delete[] array;
				}
				owner = a;
				array = a;
			}
		}

		operator T*() const {
			return array;
		}

		T* get() const {
			return array;
		}

		T* release() const {
			owner = false;
			return array;
		}

	private:
		T* array;
		mutable bool owner;
};

#endif
