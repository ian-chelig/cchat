# CChat

## Todo List

- [x] Implement CBOR
- [x] Implement error handling from the linux programming interface
- [ ] Rewrite client and server to use CBOR
- [ ] User handling
- [ ] Refactor project to use proper error handling
- [ ] Implement RSA
- [ ] Implement xz compression

## Further Goals

- [ ] Implement ffmpeg for collaborative watching and video chatting
- [ ] Implement Audio chatting
- [ ] Collaborative Coding???

### Rewriting client/server to use CBOR

- [ ] write client side handling for client side commands -- STARTED
- [ ] write server side handling for server side commands -- STARTED
- [ ] client deserialize incomming messages and hand control over to command handler

### User Handling

- [ ] change fdnode_t in server.c to the user_t
- [ ] Build and hold users in the heap

### Refactoring project to implement proper error handling

- [ ] Functions should return 0 or a positive value on success and negative on fail
- [ ] Rewrite current return values to be in/out variables instead
- [ ] Catch and handle errors using tlpi headers where necessary.
