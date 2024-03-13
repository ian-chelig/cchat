# CChat

## Todo List

- [x] Implement CBOR
- [x] Implement error handling from the linux programming interface
- [x] Rewrite client and server to use CBOR
- [ ] User handling
- [ ] Refactor project to use proper error handling
- [ ] Implement RSA
- [ ] Implement xz compression

## Further Goals

- [ ] Implement ffmpeg for collaborative watching and video chatting
- [ ] Implement Audio chatting
- [ ] Collaborative Coding???

### User Handling

- [x] change fdnode_t in server.c to the user_t
- [x] Build and hold users in the heap
- [ ] develop generateUID(){}
- [ ] may eventually need to develop an api for dealing with user_t

### Refactoring project to implement proper error handling

- [ ] Functions should return 0 or a positive value on success and negative on fail
- [ ] Rewrite current return values to be in/out variables instead
- [ ] Catch and handle errors using tlpi headers where necessary.
