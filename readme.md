# CChat

## Todo List

- [x] Implement CBOR
- [x] Implement error handling from the linux programming interface
- [x] Rewrite client and server to use CBOR
- [x] Refactor project to use proper error handling
- [ ] Garbage collection
- [ ] Memory Safety
- [ ] User handling
- [ ] Implement SHA256
- [ ] Implement RSA
- [ ] Implement xz compression

## Further Goals

- [ ] Implement ffmpeg for collaborative watching and video chatting
- [ ] Implement Audio chatting
- [ ] Collaborative Coding???

### User Handling

- [x] change fdnode_t in server.c to the user_t
- [x] Build and hold users in the heap?
- [ ] develop generateUID(){}
- [ ] may eventually need to develop an api for dealing with user_t

### Garbage collection

- [x] Client
- [ ] Server
- [ ] Struct freeing functions

### Memory Safety

- [ ] Change all syscalls to be memory safe
- [ ] Find a solution to 256 buffer sizes
- [ ] Mutexes and thread joining
- [ ] Client
- [ ] Server
