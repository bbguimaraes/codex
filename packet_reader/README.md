packet_reader
=============

Reads structured data from a packet, with proper error handling:

```cpp
void read_foo(Packet *packet) {
    struct { int x = {}, y = {}; float z = {}; } foo = {};
    // read `foo`'s fields from `packet`, handle errors
}
```

The implementation uses variadic templates and a simple data format/protocol.
Error handling uses simple `bool`s, but could be substituted with more
sophisticated approaches (the implementation of the public `get` function would
be slightly more complex).

An invalid read is caught by the conditional based on `read<Type>` and the
buffer index is left unchanged.  The failure is communicated to the
`Packet::get` caller.  An attempt to `get` an unsupported type is caught at
compile-time and generates a suitable error message based on the unsatisfied
constraints:

```
error: no matching function for call to ‘codex::Packet::get<double>(std::nullptr_t)’
     packet.get<double>(nullptr);
                               ^
note: candidate: ‘auto codex::Packet::get(auto:43* ...) [with auto:43 = {double}]’
   44 |     auto get(Readable auto *...ps) { return (this->read(ps) && ...); }
      |          ^~~
note: constraints not satisfied
In instantiation of ‘auto codex::Packet::get(auto:43* ...) [with auto:43 = {double}]’:
  required by the constraints of ‘template<class ... auto:43>  requires (... && Readable<auto:43>) auto codex::Packet::get(auto:43* ...)’
note: the expression ‘(... && Readable<auto:43>) [with auto:43 = {double}]’ evaluated to ‘false’
```
