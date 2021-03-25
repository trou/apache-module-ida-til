
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

/* -------------------------------------------------------------
 *
 * COPY PASTE FROM APR HERE
 *
 */


/* apr_table  (same for Apache 2.2 and apr 1.7 */


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

typedef struct node_header_t {
    apr_size_t size;
    apr_bucket_alloc_t *alloc;
    apr_memnode_t *memnode;
    struct node_header_t *next;
} node_header_t;

struct apr_bucket_alloc_t {
    apr_pool_t *pool;
    apr_allocator_t *allocator;
    node_header_t *freelist;
    apr_memnode_t *blocks;
};

struct cleanup_t {
    struct cleanup_t *next;
    const void *data;
    apr_status_t (*plain_cleanup_fn)(void *data);
    apr_status_t (*child_cleanup_fn)(void *data);
};

#ifndef APACHE24

struct ap_filter_provider_t {
    /** How to match this provider to filter dispatch criterion */
    enum {
        STRING_MATCH,
        STRING_CONTAINS,
        REGEX_MATCH,
        INT_EQ,
        INT_LT,
        INT_LE,
        INT_GT,
        INT_GE,
        DEFINED
    } match_type;

    /** negation on match_type */
    int not;

    /** The dispatch match itself - union member depends on match_type */
    union {
        const char *string;
        ap_regex_t *regex;
        int         number;
    } match;

    /** The filter that implements this provider */
    ap_filter_rec_t *frec;

    /** The next provider in the list */
    ap_filter_provider_t *next;

    /** Dispatch criteria for filter providers */
    enum {
        HANDLER,
        REQUEST_HEADERS,
        RESPONSE_HEADERS,
        SUBPROCESS_ENV,
        CONTENT_TYPE
    } dispatch;

    /** Match value for filter providers */
    const char* value;
};

#define MAX_INDEX 20

struct apr_allocator_t {
    /** largest used index into free[], always < MAX_INDEX */
    apr_uint32_t        max_index;
    /** Total size (in BOUNDARY_SIZE multiples) of unused memory before
     * blocks are given back. @see apr_allocator_max_free_set().
     * @note Initialized to APR_ALLOCATOR_MAX_FREE_UNLIMITED,
     * which means to never give back blocks.
     */
    apr_uint32_t        max_free_index;
    /**
     * Memory size (in BOUNDARY_SIZE multiples) that currently must be freed
     * before blocks are given back. Range: 0..max_free_index
     */
    apr_uint32_t        current_free_index;
#if APR_HAS_THREADS
    apr_thread_mutex_t *mutex;
#endif /* APR_HAS_THREADS */
    apr_pool_t         *owner;
    /**
     * Lists of free nodes. Slot 0 is used for oversized nodes,
     * and the slots 1..MAX_INDEX-1 contain nodes of sizes
     * (i+1) * BOUNDARY_SIZE. Example for BOUNDARY_INDEX == 12:
     * slot  0: nodes larger than 81920
     * slot  1: size  8192
     * slot  2: size 12288
     * ...
     * slot 19: size 81920
     */
    apr_memnode_t      *free[MAX_INDEX];
};


struct apr_pool_t {
    apr_pool_t           *parent;
    apr_pool_t           *child;
    apr_pool_t           *sibling;
    apr_pool_t          **ref;
    cleanup_t            *cleanups;
    cleanup_t            *free_cleanups;
    apr_allocator_t      *allocator;
    struct process_chain *subprocesses;
    apr_abortfunc_t       abort_fn;
    apr_hash_t           *user_data;
    const char           *tag;

#if !APR_POOL_DEBUG
    apr_memnode_t        *active;
    apr_memnode_t        *self; /* The node containing the pool itself */
    char                 *self_first_avail;

#else /* APR_POOL_DEBUG */
    apr_pool_t           *joined; /* the caller has guaranteed that this pool
                                   * will survive as long as ->joined */
    debug_node_t         *nodes;
    const char           *file_line;
    apr_uint32_t          creation_flags;
    unsigned int          stat_alloc;
    unsigned int          stat_total_alloc;
    unsigned int          stat_clear;
#if APR_HAS_THREADS
    apr_os_thread_t       owner;
    apr_thread_mutex_t   *mutex;
#endif /* APR_HAS_THREADS */
#endif /* APR_POOL_DEBUG */
#ifdef NETWARE
    apr_os_proc_t         owner_proc;
#endif /* defined(NETWARE) */
    cleanup_t            *pre_cleanups;
};
#else /* Apache 2.4 */
struct ap_filter_provider_t {
    ap_expr_info_t *expr;
    const char **types;

    /** The filter that implements this provider */
    ap_filter_rec_t *frec;

    /** The next provider in the list */
    ap_filter_provider_t *next;
};

#define MAX_INDEX 20
struct apr_allocator_t {
    /** largest used index into free[], always < MAX_INDEX */
    apr_size_t        max_index;
    /** Total size (in BOUNDARY_SIZE multiples) of unused memory before
     * blocks are given back. @see apr_allocator_max_free_set().
     * @note Initialized to APR_ALLOCATOR_MAX_FREE_UNLIMITED,
     * which means to never give back blocks.
     */
    apr_size_t        max_free_index;
    /**
     * Memory size (in BOUNDARY_SIZE multiples) that currently must be freed
     * before blocks are given back. Range: 0..max_free_index
     */
    apr_size_t        current_free_index;
#if APR_HAS_THREADS
    apr_thread_mutex_t *mutex;
#endif /* APR_HAS_THREADS */
    apr_pool_t         *owner;
    /**
     * Lists of free nodes. Slot 0 is used for oversized nodes,
     * and the slots 1..MAX_INDEX-1 contain nodes of sizes
     * (i+1) * BOUNDARY_SIZE. Example for BOUNDARY_INDEX == 12:
     * slot  0: nodes larger than 81920
     * slot  1: size  8192
     * slot  2: size 12288
     * ...
     * slot 19: size 81920
     */
    apr_memnode_t      *free[MAX_INDEX];
};
struct apr_pool_t {
    apr_pool_t           *parent;
    apr_pool_t           *child;
    apr_pool_t           *sibling;
    apr_pool_t          **ref;
    cleanup_t            *cleanups;
    cleanup_t            *free_cleanups;
    apr_allocator_t      *allocator;
    struct process_chain *subprocesses;
    apr_abortfunc_t       abort_fn;
    apr_hash_t           *user_data;
    const char           *tag;

#if !APR_POOL_DEBUG
    apr_memnode_t        *active;
    apr_memnode_t        *self; /* The node containing the pool itself */
    char                 *self_first_avail;

#else /* APR_POOL_DEBUG */
    apr_pool_t           *joined; /* the caller has guaranteed that this pool
                                   * will survive as long as ->joined */
    debug_node_t         *nodes;
    const char           *file_line;
    apr_uint32_t          creation_flags;
    unsigned int          stat_alloc;
    unsigned int          stat_total_alloc;
    unsigned int          stat_clear;
#if APR_HAS_THREADS
    apr_os_thread_t       owner;
    apr_thread_mutex_t   *mutex;
#endif /* APR_HAS_THREADS */
#endif /* APR_POOL_DEBUG */
#ifdef NETWARE
    apr_os_proc_t         owner_proc;
#endif /* defined(NETWARE) */
    cleanup_t            *pre_cleanups;
#if APR_POOL_CONCURRENCY_CHECK

#define                   IDLE        0
#define                   IN_USE      1
#define                   DESTROYED   2
    volatile apr_uint32_t in_use;
    apr_os_thread_t       in_use_by;
#endif /* APR_POOL_CONCURRENCY_CHECK */
};

struct ap_filter_provider_t {
    ap_expr_info_t *expr;
    const char **types;

    /** The filter that implements this provider */
    ap_filter_rec_t *frec;

    /** The next provider in the list */
    ap_filter_provider_t *next;
};
#endif

