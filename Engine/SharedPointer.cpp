#include "SharedPointer.hpp"

SharedPointerData<void> _nullPtrData(nullptr, 0);

template<typename T>
const FunctionPointer<void, SharedPointerData<T>&> SharedPointer<T>::_pDeletePointer =
	FunctionPointer<void, SharedPointerData<T>&>(&SharedPointer<T>::_DeletePointer);
