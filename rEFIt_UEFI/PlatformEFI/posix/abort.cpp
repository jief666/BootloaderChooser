
#include <Platform.h> // Only use angled for Platform, else, xcode project won't compile


#ifdef __cplusplus
extern "C" {
#endif

#if defined(CLOVER_BUILD) || !defined(_MSC_VER)
void abort(void)
{
  printf("A fatal error happened. System halted\n");
  while (1) { // tis will avoid warning : Function declared 'noreturn' should not return
    CpuDeadLoop();
  }
}
#endif

#ifdef __cplusplus
}
#endif

bool stop_at_panic = true;
bool i_have_panicked = false;

/*
 *
 * Function panic_ seems useless. It's same as panic(). It's to be able to put a breakpoint in gdb with br panic_(). This is done in gdb_launch script in Qemu
 */
static void panic_(const char* format, VA_LIST va)
#ifndef PANIC_CAN_RETURN
    __attribute__ ((noreturn));
#endif
;

#ifdef CLOVER_BUILD
//extern void egSetGraphicsModeEnabled(XBool);
//extern const LString8 gBuildId;
#endif

#define FATAL_ERROR_MSG "\nA fatal error happened. System halted.\n"
static void panic_(const char* format, VA_LIST va)
{
#ifdef CLOVER_BUILD
//  egSetGraphicsModeEnabled(false);
//  printf("Clover build id: %s\n", gBuildId.c_str());
#endif
  if ( format ) {
//    vprintf(format, va);
//    #ifdef DEBUG_ON_SERIAL_PORT
//      char buf[500];
//      vsnprintf(buf, sizeof(buf)-1, format, va);
//      SerialPortWrite((UINT8*)buf, strlen(buf));
//    #endif
      char buf[500];
      vsnprintf(buf, sizeof(buf)-1, format, va);
      DebugLog(2, "%s", buf);
  }
//  printf(FATAL_ERROR_MSG);
//  #ifdef DEBUG_ON_SERIAL_PORT
//    SerialPortWrite((UINT8*)FATAL_ERROR_MSG, strlen(FATAL_ERROR_MSG));
//  #endif
  DebugLog(2, "%s", FATAL_ERROR_MSG);
  DebugLog(2, "\n");
  while (1) { // this will avoid warning : Function declared 'noreturn' should not return
    CpuDeadLoop();
  }
}

void panic(const char* format, ...)
{
#ifdef PANIC_CAN_RETURN
  if ( stop_at_panic ) {
    VA_LIST va;
    VA_START(va, format);
    panic_(format, va); // panic doesn't return
//    VA_END(va);
  }else{
    i_have_panicked = true;
  }
#else
  VA_LIST va;
  VA_START(va, format);
  panic_(format, va); // panic doesn't return
#endif
}


/*
 * Future version to log about pontential technical bugs
 * It's not done yes. So far, it's just panic
 * TODO:
 */
static void panic__(const char* format, VA_LIST va)
{
#ifdef CLOVER_BUILD
//  egSetGraphicsModeEnabled(false);
//  printf("Clover build id: %s\n", gBuildId.c_str());
#endif
  if ( format ) {
//    vprintf(format, va);
//    #ifdef DEBUG_ON_SERIAL_PORT
//      char buf[500];
//      vsnprintf(buf, sizeof(buf)-1, format, va);
//      SerialPortWrite((UINT8*)buf, strlen(buf));
//    #endif
      char buf[500];
      vsnprintf(buf, sizeof(buf)-1, format, va);
      DebugLog(2, "%s", buf);
  }
//  printf(FATAL_ERROR_MSG);
//  #ifdef DEBUG_ON_SERIAL_PORT
//    SerialPortWrite((UINT8*)FATAL_ERROR_MSG, strlen(FATAL_ERROR_MSG));
//  #endif
  DebugLog(2, "%s", FATAL_ERROR_MSG);
  DebugLog(2, "\n");
  for (size_t i = 0 ; i < SIZE_T_MAX ; i++ ) { // this will avoid warning : Function declared 'noreturn' should not return
    CpuDeadLoop();
  }
}

void log_technical_bug(const char* format, ...)
{
#ifdef PANIC_CAN_RETURN
  if ( stop_at_panic ) {
    VA_LIST va;
    VA_START(va, format);
    panic__(format, va); // panic doesn't return
//    VA_END(va);
  }else{
    i_have_panicked = true;
  }
#else
  VA_LIST va;
  VA_START(va, format);
  panic__(format, va); // panic doesn't return
#endif
}


void panic(void)
{
  panic(nullptr);
}


void _assert(bool b, const char* format, ...)
{
  if ( !b ) {
    VA_LIST va;
    VA_START(va, format);
    panic_(format, va); // panic doesn't return
  }
}
