event_dispatcher
================

Based on [this article][0] with a generic implementation for Rust.  The article
uses the `std::intrinsics::type_id` compiler intrinsic to create a hash map from
types to vectors.  This C++ implementation instead makes the supported event
types part of the dispatcher type.

[0]: https://willcrichton.net/notes/types-over-strings
