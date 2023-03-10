# Overview

flat\_map is a header only associative container library that constructed on linear container.
It compliants C++17/20 standard associative container requirements except complexity and interator/reference invalidation.

flat\_map is well known replacement of std::map to improve scan access performance.

## Requirements

- Fully C++17 conformant compiler and standard library

## Usage

This library is header only library.
No building and installing are not required.

## Thread safety

This library provides no special guarantee for inter-thread operations except read-only operations.

## License

This library distributed under MIT license, see [LICENSE](../LICENSE) for more detail.

## Other implementations

Followings are the library that provides same 'flat\_map' idiom.

- [A standard flat\_map](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0429r9.pdf)
- [Boost.Container - flat_(multi)map/set associative containers](https://www.boost.org/doc/html/container/non_standard_containers.html#container.non_standard_containers.flat_xxx)
- ETL [flat\_map / flat\_multimap](https://www.etlcpp.com/flat_map.html) [flat\_set / flat\_multiset](https://www.etlcpp.com/flat_set.html)
- chromium [base/containers library](https://chromium.googlesource.com/chromium/src/+/HEAD/base/containers/README.md#base_flat_map-and-base_flat_set)
