#if defined(TI_IS_MSVC)
  #define PRAGMA_WARNING_PUSH_MSVC() __pragma(warning(push))
  #define PRAGMA_WARNING_POP_MSVC() __pragma(warning(pop))
  #define PRAGMA_WARNING_IGNORE_MSVC(Warning) __pragma(warning(disable : Warning))

  #define WARNING_MSVC_THIS_FUNCTION_OR_VARIABLE_MAY_BE_UNSAFE 4996

  #define PRAGMA_WARNING_PUSH_GCC()
  #define PRAGMA_WARNING_POP_GCC()
  #define PRAGMA_WARNING_IGNORE_GCC(Warning)

  #define WARNING_IGNORED_ATTRIBUTES_GCC
#elif defined(TI_IS_CLANG_CL) || defined(TI_IS_GCC) || defined(TI_IS_CLANG)
  #define PRAGMA_WARNING_PUSH_MSVC()
  #define PRAGMA_WARNING_POP_MSVC()
  #define PRAGMA_WARNING_IGNORE_MSVC(Warning)

  #define WARNING_MSVC_THIS_FUNCTION_OR_VARIABLE_MAY_BE_UNSAFE

  #define PRAGMA_WARNING_PUSH_GCC() _Pragma(GCC diagnostic push)
  #define PRAGMA_WARNING_POP_GCC() _Pragma(GCC diagnostic pop)
  #define PRAGMA_WARNING_IGNORE_GCC(Warning) _Pragma(GCC diagnostic ignored #Warning)

  #define WARNING_IGNORED_ATTRIBUTES_GCC                                                                               \
    -Wattributes // NOLINT(cppcoreguidelines-macro-usage,bugprone-macro-parentheses)
#endif

#if defined(TI_IS_CLANG_CL) || defined(TI_IS_CLANG)
  #define CLANG_MAYBE_UNUSED [[maybe_unused]] // NOLINT(cppcoreguidelines-macro-usage)
#else
  #define CLANG_MAYBE_UNUSED
#endif
