/*
 * basic_kalman.c
 *
 *  Created on: 14 Eyl 2022
 *      Author: b1d0
 */

//Defines
static const double R = 40; // noise coavirance (normally 10)
static const double H = 1.00; //measurment map scalar
static double Q = 10; //initial estimated covariance
static double P = 0; //initial error covariance (it must be 0)
static double U_hat = 0; //initial estimated state
static double K = 0; //initial kalman gain

/*In this function, U_hat is the starting point.
 * If you measure the pressure with Pascal, U_hat should equal to 101325,
 * or if you measure the temperature with Celsius, you should start with 25.
 */

double Kalman (double U)
{
	K = P * H / (H * P * H + R);
	U_hat = U_hat + K * (U - H * U_hat);
	P = (1 - K * H) * P + Q;

	return U_hat;
}

