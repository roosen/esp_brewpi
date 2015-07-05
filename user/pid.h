#ifndef _PID_H_
#define _PID_H_


/* Constants used in some of the functions below */
#define MANUAL     0
#define AUTOMATIC  1
#define DIRECT     0
#define REVERSE    1


struct pid_t {
	double kp; /* (P)roportional Tuning Parameter */
	double ki; /* (I)ntegral Tuning Parameter     */
	double kd; /* (D)erivative Tuning Parameter   */

	int controllerDirection;

	/*
	 * Pointers to the Input, Output, and Setpoint variables
	 * This creates a hard link between the variables and the
	 * PID, freeing the user from having to constantly tell us
	 * what these values are.  with pointers we'll just know.
	 */
	double *myInput;
	double *myOutput;
	double *mySetpoint;

	unsigned long lastTime;
	double ITerm, lastInput;

	double outMin, outMax;
	int inAuto;
};


/* commonly used functions **************************************************************************/
/*
 * constructor.  links the PID to the Input, Output, and
 * Setpoint.  Initial tuning parameters are also set here
 */
void PID_Create(struct pid_t *pid, double*, double*, double*, double, double, double, int);

/* sets PID to either Manual (0) or Auto (non-0) */
void PID_SetMode(struct pid_t *pid, int Mode);

/*
 * performs the PID calculation.  it should be
 * called every time loop() cycles. ON/OFF and
 * calculation frequency can be set using SetMode
 * SetSampleTime respectively
 */
int PID_Compute(struct pid_t *pid);

/*
 * clamps the output to a specific range. 0-255 by default, but
 * it's likely the user will want to change this depending on
 * the application
 */
void PID_SetOutputLimits(struct pid_t *pid, double, double);


/* available but not commonly used functions ********************************************************/
/*
 * While most users will set the tunings once in the
 * constructor, this function gives the user the option
 * of changing tunings during runtime for Adaptive control
 */
void PID_SetTunings(struct pid_t *pid, double, double, double);

/*
 * Sets the Direction, or "Action" of the controller. DIRECT
 * means the output will increase when error is positive. REVERSE
 * means the opposite.  it's very unlikely that this will be needed
 * once it is set in the constructor.
 */
void PID_SetControllerDirection(struct pid_t *pid, int);


/* Display functions ****************************************************************/
/*
 * These functions query the pid for interal values.
 * they were created mainly for the pid front-end,
 * where it's important to know what is actually
 * inside the PID.
 */
double PID_GetKp(struct pid_t *pid);
double PID_GetKi(struct pid_t *pid);
double PID_GetKd(struct pid_t *pid);
int PID_GetMode(struct pid_t *pid);
int PID_GetDirection(struct pid_t *pid);

#endif /* _PID_H_ */
