#ifndef _COMMON_H_
# define _COMMON_H_

/*
	Status define the state of the command triggered by the Telegram bot or Serial UART tx/rx
	value:
		0. -> GrinderHandler stop motor
		1 -> GrinderHandler start notor for the amount of GrinderState.seconds time.
		2. -> GrinderHandler start  motor for 1s
		3. -> maintenance mode without message
		4. -> Not container, GrinderHandler in 'maintenance mode' waiting a signal to unlock it
		5. -> No more beans, GrinderHandler in 'maintenance mode' waiting a signal to unlock it
*/

struct GrinderState
{
	unsigned short status;
	unsigned int seconds;
};

#endif
