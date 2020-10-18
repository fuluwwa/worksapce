// xhashtab  defination
//
#include "queue.h"
#include <math.h>
#if 0
#define HASH_FUNC hash_str
#define HASH_CMP strcmp
#else
#define HASH_FUNC hash_ptr                      // 定义hash函数
#define HASH_CMP  ptrcmp                        // 定义key的比较函数
#endif

static inline int ptrcmp(void *a, void *b)
{
        return a==b?0:1;
}

typedef unsigned int u32;
/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
static inline u32 hash_32(u32 val, unsigned int bits)
{
        /* On some cpus multiply is faster, on others gcc will do shifts */
        u32 hash = val * GOLDEN_RATIO_PRIME_32;

        /* High bits are more random, so use them. */
        return hash >> (32 - bits);
}

// 指针的hash函数
static inline u32 hash_ptr(void *ptr, unsigned int bits)
{
        return hash_32((u32)ptr, bits);
}

// 字符串的hash函数
static unsigned long hash_str(const char *str, unsigned int bits)
{
        unsigned long hash = 5381;
        int c;

        while (c = *str++) {
                hash = ((hash << 5) + hash) + c;
        }
        return hash >> (32 - bits);
}

// bucket头定义，hashtab就是bucket的数组
#define HLIST_HEAD(name, type)                                           \
struct name {                                                           \
        struct type *lh_first;  /* first element */                     \
        int count;                                                      \
}

// bucket初始化
#define HLIST_INIT(head) do {                                            \
        (head)->lh_first = NULL;                                        \
        (head)->count = 0;                                              \
} while (/*CONSTCOND*/0)

//hash节点元素定义
struct hash_elm{
        void *key;
        void *val;
        LIST_ENTRY(hash_elm) entry;
};

//hash bucket
#define HASH_HEAD struct hash_head
//hash element
#define HASH_ELM struct hash_elm

// 定义一个hashtab, 2^bits大小
#define DEFINE_HASHTABLE(name, bits)                                            \
        HLIST_HEAD(hash_head, hash_elm) name[1 << (bits)] =                                   \
                        { [0 ... ((1 << (bits)) - 1)] = { .lh_first = NULL, .count = 0 }}
//声明
#define DECLARE_HASHTABLE(name, bits)                                           \
        HLIST_HEAD(hash_head, hash_elm) name[1 << (bits)]

// hashtab数组大小
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HASH_SIZE(name) (ARRAY_SIZE(name))

// hash数组的大小用几个bits可以表示
#define HASH_BITS(name) log2(HASH_SIZE(name))

// 元素并没有hash
#define LIST_UNHASHED(elm)       (!(elm)->entry.le_prev)

// 包装一层为了统计数目
#define HLIST_INSERT_HEAD(head, elm, entry)     do {                                                            \
        (head)->count++;LIST_INSERT_HEAD(head, elm, entry);                                                     \
}while (0)

// hash出桶链表，插入链头
#define HASH_ADD(hashtab, elm)  HLIST_INSERT_HEAD(&hashtab[HASH_FUNC((elm)->key, HASH_BITS(hashtab))], elm, entry)

// 删除元素
#define HASH_DEL(hashtab, elm)  do {                                            \
        if (elm && !LIST_UNHASHED(elm)) {                                       \
                LIST_REMOVE(elm, entry);                                        \
                hashtab[HASH_FUNC((elm)->key, HASH_BITS(hashtab))].count--;     \
        }                                                                       \
}while (0)

// 根据key, 查找元素指针
#define HASH_FIND(hashtab, _key)         ({                                                             \
        struct hash_elm *elm;                                                                           \
        LIST_FOREACH(elm, &hashtab[HASH_FUNC(_key, HASH_BITS(hashtab))], entry) {                       \
                if (HASH_CMP(_key, elm->key) == 0)                                                      \
                        break;                                                                          \
        }                                                                                               \
        elm;                                                                                            \
})

// 初始化hashtab
#define HASH_INIT(hashtab)      do {                                    \
        unsigned int i;                                                 \
                                                                        \
        for (i = 0; i < HASH_SIZE(hashtab); i++)                        \
                HLIST_INIT(&hashtab[i]);                                        \
}while (0)

// hashtab 为空, 有返回值加()
#define HASH_EMPTY(hashtab)     ({                                      \
        unsigned int i;                                                 \
                                                                        \
        for (i = 0; i < HASH_SIZE(hashtab); i++)                        \
                if (!LIST_EMPTY(&hashtab[i]))                           \
                        break;                                          \
        i == HASH_SIZE(hashtab);                                        \
})

// 遍历整个hashtab, bkt是桶的index, elm是hash_elm的临时变量
#define HASH_FOREACH(hashtab, bkt, elm)                                 \
        for ((bkt) = 0, elm = NULL; elm == NULL && (bkt) < HASH_SIZE(hashtab);\
                        (bkt)++)\
                LIST_FOREACH(elm, &hashtab[bkt], entry)

// 遍历bucket
#define HASH_FOREACH_BKT(hashtab, bkt)                                  \
        for ((bkt) = 0; (bkt) < HASH_SIZE(hashtab); (bkt)++)

// 遍历元素
#define HASH_FOREACH_ELM(head, elm)                                     \
                LIST_FOREACH(elm, head, entry)

