#ifndef __FILTER_H
#define __FILTER_H

extern float angle, angle_dot; 	
void Kalman_Filter(float Accel,float Gyro);
#endif
