#if _IMPLEMENT

#define _EXT
#define _INIT( ... ) \
	= __VA_ARGS__

#ifdef __cplusplus
#define _EXT_C \
	extern "C"
#else
#define _EXT_C \
	extern
#endif

#define _VAR( decl ) \
	decl
#define _VAR_INIT( decl, ... ) \
	decl = __VA_ARGS__

#ifdef __cplusplus
#define _VAR_C( decl ) \
	extern "C" decl; decl
#define _VAR_C_INIT( decl, ... ) \
	extern "C" decl; decl = __VA_ARGS__
#else
#define _VAR_C( decl ) \
	decl
#define _VAR_C_INIT( decl, ... ) \
	decl = __VA_ARGS__
#endif

#define _INL

#else  /* _IMPLEMENT */

#define _EXT \
	extern
#define _INIT( ... )

#ifdef __cplusplus
#define _EXT_C \
	extern "C"
#else
#define _EXT_C \
	extern
#endif

#define _VAR( decl ) \
	extern decl
#define _VAR_INIT( decl, ... ) \
	extern decl

#ifdef __cplusplus
#define _VAR_C( decl ) \
	extern "C" decl
#define _VAR_C_INIT( decl, ... ) \
	extern "C" decl
#else
#define _VAR_C( decl ) \
	extern decl
#define _VAR_C_INIT( decl, ... ) \
	extern decl
#endif

#if __GNUC__ && !__GNUC_STDC_INLINE__
#define _INL \
	extern inline
#else
#define _INL \
	inline
#endif

#endif /* _IMPLEMENT */
