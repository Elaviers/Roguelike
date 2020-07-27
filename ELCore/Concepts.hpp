#pragma once
#include <type_traits>

namespace Concepts
{
	template <class Derived, class Base>
	concept DerivedFrom = std::is_base_of<Base, Derived>::value;
}
