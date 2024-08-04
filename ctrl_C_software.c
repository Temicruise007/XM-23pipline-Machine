#include"Emulator.h"

/************ Globals inside debugger ************/



/*
 - volatile - do not cache or optimize, it can be changed elsewhere
 - sig_atomic_t - shared variable - handle possible race conditions
*/

volatile sig_atomic_t ctrl_c_fnd; /* T|F - indicates whether ^C detected */

/************ Debugger support function ************/

void sigint_hdlr()
{
	/*
	- Invoked when SIGINT (control-C) is detected
	- changes state of waiting_for_signal
	- signal must be reinitialized
	*/
	ctrl_c_fnd = TRUE;
	signal(SIGINT, (_crt_signal_t)sigint_hdlr); /* Reinitialize SIGINT */

	// Clear input buffer
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
}

/************ Debugger startup software ************/

/* Call signal() - bind sigint_hdlr to SIGINT */
void init_signal() {

	ctrl_c_fnd = FALSE;
	signal(SIGINT, (_crt_signal_t)sigint_hdlr);

}

/************ Before calling the CPU emulator *************/
void run_xm()
{
	/* Run the CPU */
	ctrl_c_fnd = FALSE;
	CPU();
}