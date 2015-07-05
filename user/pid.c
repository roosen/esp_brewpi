#include <user_interface.h>
#include "pid.h"


/* Initialize()****************************************************************
 *	does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/
static void PID_Initialize(struct pid_t *pid)
{
	pid->ITerm = *pid->myOutput;
	pid->lastInput = *pid->myInput;
	if (pid->ITerm > pid->outMax)
		pid->ITerm = pid->outMax;
	else if (pid->ITerm < pid->outMin)
		pid->ITerm = pid->outMin;
}


/* Constructor (...)********************************************************
 *    The parameters specified here are those for for which we can't set up 
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
void PID_Create(struct pid_t *pid, double* Input, double* Output,
		double* Setpoint, double Kp, double Ki, double Kd,
		int ControllerDirection)
{

	pid->myOutput = Output;
	pid->myInput = Input;
	pid->mySetpoint = Setpoint;
	pid->inAuto = 0;

	/*
	 * default output limit corresponds to
	 * the arduino pwm limits
	 */
	PID_SetOutputLimits(pid, 0, 255);

	/* default Controller Sample Time is 0.1 seconds */
	pid->SampleTime = 100;

	PID_SetControllerDirection(pid, ControllerDirection);
	PID_SetTunings(pid, Kp, Ki, Kd);

	pid->lastTime = system_get_time() - pid->SampleTime;
}

/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/
int PID_Compute(struct pid_t *pid)
{
	unsigned long now;
	unsigned long timeChange;

	if (!pid->inAuto)
		return 0;

	now = system_get_time();
	timeChange = (now - pid->lastTime);

	if (timeChange >= pid->SampleTime) {
		/* Compute all the working error variables */
		double input = *pid->myInput;
		double error = *pid->mySetpoint - input;
		double dInput;
		double output;

		pid->ITerm += (pid->ki * error);
		if (pid->ITerm > pid->outMax)
			pid->ITerm = pid->outMax;
		else if (pid->ITerm < pid->outMin)
			pid->ITerm = pid->outMin;

		dInput = (input - pid->lastInput);

		/* Compute PID Output */
		output = pid->kp * error + pid->ITerm - pid->kd * dInput;

		if (output > pid->outMax)
			output = pid->outMax;
		else if (output < pid->outMin)
			output = pid->outMin;
		*pid->myOutput = output;

		/* Remember some variables for next time */
		pid->lastInput = input;
		pid->lastTime = now;
		return 1;
	} else {
		return 0;
	}
}

/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted. 
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 ******************************************************************************/
void PID_SetTunings(struct pid_t *pid, double Kp, double Ki, double Kd)
{
	double SampleTimeInSec;

	if (Kp < 0 || Ki < 0 || Kd < 0)
		return;

	pid->dispKp = Kp;
	pid->dispKi = Ki;
	pid->dispKd = Kd;

	SampleTimeInSec = ((double) pid->SampleTime) / 1000;
	pid->kp = Kp;
	pid->ki = Ki * SampleTimeInSec;
	pid->kd = Kd / SampleTimeInSec;

	if (pid->controllerDirection == REVERSE) {
		pid->kp = (0 - pid->kp);
		pid->ki = (0 - pid->ki);
		pid->kd = (0 - pid->kd);
	}
}

/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed	
 ******************************************************************************/
void PID_SetSampleTime(struct pid_t *pid, int NewSampleTime)
{
	if (NewSampleTime > 0) {
		double ratio = (double) NewSampleTime / (double) pid->SampleTime;
		pid->ki *= ratio;
		pid->kd /= ratio;
		pid->SampleTime = (unsigned long) NewSampleTime;
	}
}

/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 **************************************************************************/
void PID_SetOutputLimits(struct pid_t *pid, double Min, double Max)
{
	if (Min >= Max)
		return;

	pid->outMin = Min;
	pid->outMax = Max;

	if (pid->inAuto) {
		if (*pid->myOutput > pid->outMax)
			*pid->myOutput = pid->outMax;
		else if (*pid->myOutput < pid->outMin)
			*pid->myOutput = pid->outMin;

		if (pid->ITerm > pid->outMax)
			pid->ITerm = pid->outMax;
		else if (pid->ITerm < pid->outMin)
			pid->ITerm = pid->outMin;
	}
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/
void PID_SetMode(struct pid_t *pid, int Mode)
{
	int newAuto = (Mode == AUTOMATIC);
	if (newAuto == !pid->inAuto) {
		/* we just went from manual to auto */
		PID_Initialize(pid);
	}
	pid->inAuto = newAuto;
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads 
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void PID_SetControllerDirection(struct pid_t *pid, int Direction)
{
	if (pid->inAuto && Direction != pid->controllerDirection) {
		pid->kp = (0 - pid->kp);
		pid->ki = (0 - pid->ki);
		pid->kd = (0 - pid->kd);
	}
	pid->controllerDirection = Direction;
}

/* Status Funcions*************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display 
 * purposes.  this are the functions the PID Front-end uses for example
 ******************************************************************************/
double PID_GetKp(struct pid_t *pid)
{
	return pid->dispKp;
}

double PID_GetKi(struct pid_t *pid)
{
	return pid->dispKi;
}

double PID_GetKd(struct pid_t *pid)
{
	return pid->dispKd;
}

int PID_GetMode(struct pid_t *pid)
{
	return pid->inAuto ? AUTOMATIC : MANUAL;
}

int PID_GetDirection(struct pid_t *pid)
{
	return pid->controllerDirection;
}
