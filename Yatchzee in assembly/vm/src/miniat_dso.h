#ifndef VM_SRC_MINIAT_DSO_H_
#define VM_SRC_MINIAT_DSO_H_

#if defined _WIN32 || defined __CYGWIN__
  #define PUBLIC
  #define PRIVATE
#else
  #if __GNUC__ >= 4
    #define PUBLIC __attribute__ ((visibility ("default")))
    #define PRIVATE  __attribute__ ((visibility ("hidden")))
  #else
    #define PUBLIC
    #define PRIVATE
  #endif
#endif

#endif /* VM_SRC_MINIAT_DSO_H_ */
