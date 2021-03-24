
/* Used to "nop" __asm__ macros which are not supported by
 * tilib
 */
#define __asm__(arg) 

#include <apr.h>
#include <apr_buckets.h>
#include <apr_optional.h>
#include <apr_allocator.h>
#include <apr_atomic.h>
#include <apr_dso.h>
#include <apr_env.h>
#include <apr_errno.h>
#include <apr_escape.h>
#include <apr_file_info.h>
#include <apr_file_io.h>
#include <apr_fnmatch.h>
#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_global_mutex.h>
#include <apr_hash.h>
#include <apr_inherit.h>
#include <apr_lib.h>
#include <apr_mmap.h>
#include <apr_network_io.h>
#include <apr_poll.h>
#include <apr_pools.h>
#include <apr_portable.h>
#include <apr_proc_mutex.h>
#include <apr_random.h>
#include <apr_ring.h>
#include <apr_shm.h>
#include <apr_signal.h>
#include <apr_skiplist.h>
#include <apr_strings.h>
#include <apr_support.h>
#include <apr_tables.h>
#include <apr_thread_cond.h>
#include <apr_thread_mutex.h>
#include <apr_thread_proc.h>
#include <apr_thread_rwlock.h>
#include <apr_time.h>
#include <apr_user.h>
#include <apr_version.h>
#include <apr_want.h>
#ifdef APACHE24
#include <apr_perms_set.h>
#include <apr_cstr.h>
#include <apr_encode.h>
#endif


#include <ap_config.h>
#include <ap_provider.h>
#include <httpd.h>
#include <http_core.h>
#include <http_config.h>
#include <http_log.h>
#include <http_protocol.h>
#include <http_request.h>

#define TABLE_HASH_SIZE 32
#define TABLE_INDEX_MASK 0x1f
#define TABLE_HASH(key)  (TABLE_INDEX_MASK & *(unsigned char *)(key))
#define TABLE_INDEX_IS_INITIALIZED(t, i) ((t)->index_initialized & (1 << (i)))
#define TABLE_SET_INDEX_INITIALIZED(t, i) ((t)->index_initialized |= (1 << (i)))

// Damn structure is "opaque"

struct apr_table_t {
    /* This has to be first to promote backwards compatibility with
     * older modules which cast a apr_table_t * to an apr_array_header_t *...
     * they should use the apr_table_elts() function for most of the
     * cases they do this for.
     */
    /** The underlying array for the table */
    apr_array_header_t a;
#ifdef MAKE_TABLE_PROFILE
    /** Who created the array. */
    void *creator;
#endif
    /* An index to speed up table lookups.  The way this works is:
     *   - Hash the key into the index:
     *     - index_first[TABLE_HASH(key)] is the offset within
     *       the table of the first entry with that key
     *     - index_last[TABLE_HASH(key)] is the offset within
     *       the table of the last entry with that key
     *   - If (and only if) there is no entry in the table whose
     *     key hashes to index element i, then the i'th bit
     *     of index_initialized will be zero.  (Check this before
     *     trying to use index_first[i] or index_last[i]!)
     */
    apr_uint32_t index_initialized;
    int index_first[TABLE_HASH_SIZE];
    int index_last[TABLE_HASH_SIZE];
};
