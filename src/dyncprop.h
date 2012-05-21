
/**
 * dynamic constant propagation function
 * 
 * the format string determines the arguments of the function:
 *   $x => a symbolic argument
 *   %x => a real argument (with value passed as a vararg)
 * 
 * possible values for x:
 *   u, d => integer (signed or unsigned) [4 bytes]
 *   f => floating point [4 bytes]
 *   p => pointer [4 bytes]
 */
void* dyncprop(void* pfn, const char* format, ...);



