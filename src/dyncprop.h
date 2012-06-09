
/**
 * dynamic constant propagation function
 * 
 * the format string determines the arguments of the function:
 *   X (capital) => a symbolic argument
 *   x (lowercase) => a real argument (with value passed as a vararg)
 * 
 * the length of the string is the number of 4-byte arguments of the function
 * (only 4-byte arguments will be supported at this time)
 * 
 * possible values for x:
 *   u, d => integer (signed or unsigned) [4 bytes]
 *   f => floating point [4 bytes]
 *   p => pointer [4 bytes]
 */

#ifdef __cplusplus
extern "C"
#endif
void* dyncprop(void* pfn, const char* format, ...);



