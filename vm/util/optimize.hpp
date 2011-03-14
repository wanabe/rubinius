// Some typical optimization tricks.

// This one provides the compiler about branch hints, so it
// keeps the normal case fast.
#ifdef __GNUC__

#ifdef _WIN64
#define likely(x)       __builtin_expect((long long int)(x),1)
#define unlikely(x)     __builtin_expect((long long int)(x),0)
#else
#define likely(x)       __builtin_expect((long int)(x),1)
#define unlikely(x)     __builtin_expect((long int)(x),0)
#endif

#else

#define likely(x) x
#define unlikely(x) x

#endif

