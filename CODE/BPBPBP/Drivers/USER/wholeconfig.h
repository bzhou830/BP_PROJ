#ifndef __WHOLECONFIG_H__
#define __WHOLECONFIG_H__


// <<< Use Configuration Wizard in Context Menu >>>\n

// <h> FUNCTION ENABLE/DISABLE

// <q> DEBUG_ENABLE
#define DEBUG_ENABLE     1





// </h>





// <<< end of configuration section >>>



#if DEBUG_ENABLE
#define b_log(...)		printf(__VA_ARGS__)
#else
#define b_log(...)
#endif














#endif

