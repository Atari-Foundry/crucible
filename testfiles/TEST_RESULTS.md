# Crucible Test Results

## Test Files Created

### XEX Programs
- **hello.xex**: Simple "HELLO" program using CIO
- **read_h.xex**: Program to read from H: device (host filesystem)

### ATR Disk Images
- **test.atr**: Empty 720-sector ATR image (128 bytes/sector)

### Test Data
- **TEST.TXT**: Test file for H: device access

## Test Results

### ATR Image Loading
✅ **PASS**: ATR image loads successfully
```bash
./build/crucible -I testfiles/test.atr
```
Result: Image loads without errors (empty image cannot boot, which is expected)

### XEX Program Loading
✅ **PASS**: XEX programs now work correctly
```bash
./build/crucible testfiles/hello.xex
```
Result: Program loads and executes successfully, prints "HELLO"
- Fixed: RTS callback simulation in `sim_CIOV` now properly handles return addresses
- Fixed: Stack preservation in `call_devtab` prevents corruption from nested calls
- Programs correctly use CIOV for screen output

### H: Device Test
✅ **PASS**: H: device program now works correctly
```bash
./build/crucible -R testfiles testfiles/read_h.xex
```
Result: Program loads and can access H: device (host filesystem)
- Fixed: Same RTS callback and stack preservation fixes as hello.xex
- Program successfully opens files from host filesystem via H: device

## Notes

1. **ATR Format**: The ATR creation script now correctly generates ATR headers that crucible can load.

2. **CIO Issues**: ✅ **FIXED** - All CIO issues resolved:
   - Set ICCOM command using absolute addressing (STA $0342)
   - Store character in CIOCHR ($2F) before PUT CHAR operations
   - CIOV callback executes correctly and can output characters
   - **Fixed**: RTS callback simulation - `sim_CIOV` now simulates RTS instruction by popping return address from stack and updating PC
   - **Fixed**: Stack preservation in `call_devtab` - saves and restores return address to prevent corruption from nested `sim65_call`
   - All test programs now execute successfully

3. **Next Steps for Better Testing**:
   - Create ATR images with actual DOS and test files
   - Use real Atari programs or BASIC programs
   - Test directory listing functionality
   - Test file reading from ATR filesystem

## Tools Created

- **create_xex.py**: Creates XEX files from hex data
- **create_simple_xex.py**: Creates simple test XEX programs
- **create_atr.py**: Creates ATR disk images

All tools are functional and create valid file formats that crucible can load.
