#include <stdio.h>
#include "brick.h"
#include <unistd.h>

#define Sleep( msec ) usleep(( msec ) * 1000 ) /* Definerar sleep, Sleep(1000)= 1 sekund */

#define MOTOR_RIGHT    	OUTA
#define MOTOR_LEFT    	OUTB
#define MOTOR_C    		OUTC
#define MOTOR_D    		OUTD
#define SENSOR_TOUCH	IN1
#define SENSOR_2	IN2
#define SENSOR_3	IN3
#define SENSOR_4	IN4

#define MOTOR_BOTH     	( MOTOR_LEFT | MOTOR_RIGHT ) /* Bitvis ELLER ger att båda motorerna styrs samtidigt */

int max_hastighet;         /* variabel för max hastighet på motorn */
POOL_T touchSensor;

	POOL_T sensor_us;
	int  usValue = 0;
	int min_distance=4000;

	POOL_T gyroSensor;
	int gyroValue0 = 0;
	int gyroValue1 = 0;  

	int turn(void){
	int minVal = 2147483647,degrees,data;
	us_set_mode_us_dist_cm(sensor_us);
	sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_G_AND_A);
	for(int i = 0; i<360;i++){
		//skriver data från sensor till lista
		data = sensor_get_value(0, sensor_us, 0);
		if(minVal > data){
			minVal = data;
			degrees = i;
		}
		//snurrar 1 grad
		while(sensor_get_value(0, gyroSensor, 0) < i){
			tacho_set_speed_sp(MOTOR_RIGHT, max_hastighet *( -1));
        		tacho_set_speed_sp(MOTOR_LEFT, max_hastighet * 1);
			tacho_run_forever(  MOTOR_BOTH );
                	Sleep(50);
			tacho_stop(MOTOR_BOTH);
		}
	}
	if(degrees < 180)
		return degrees;
	else
		return degrees - 360;
}

int main( void )
{  
	if ( !brick_init()) return ( 1 ); /* Initialiserar EV3-klossen */
	printf( "*** ( EV3 ) Hello! ***\n" );
	Sleep( 2000 );
	
	if ( tacho_is_plugged( MOTOR_BOTH, TACHO_TYPE__NONE_ )) 
	{ 
        	max_hastighet = tacho_get_max_speed( MOTOR_LEFT, 0 );	/* Kollar maxhastigheten som motorn kan ha */
        	tacho_reset( MOTOR_BOTH );
    	} 
	else 
	{
		printf( "Anslut vänster motor i port A,\n"
		       "Anslut höger motor i port B.\n");
		
	}
      	brick_uninit();
        return ( 0 );  /* Stänger av sig om motorer ej är inkopplade */
    	}
	
	int rotate = turn();
	while(sensor_get_value(0, gyroSensor, 0) < rotate){
		tacho_set_speed_sp(MOTOR_RIGHT, max_hastighet *( -1));
		tacho_set_speed_sp(MOTOR_LEFT, max_hastighet * 1);
		tacho_run_forever(  MOTOR_BOTH );
		Sleep(50);
		tacho_stop(MOTOR_BOTH);
	}


	touchSensor = sensor_search( LEGO_EV3_TOUCH ); // Registrerar en touch sensor på touchSensor-variabeln
	touch_set_mode_touch(touchSensor); // anger vilken "mode" sensorn skall ha

	tacho_set_speed_sp( MOTOR_BOTH, max_hastighet * 0.5 );  // Sätter hastigheten på båda motorerna till 50% av maxhastigheten
	/* Om man vill köra bakåt anger man negativ hastighet, till exempel max_hastighet * (-0.5) */
	
	tacho_run_forever(  MOTOR_BOTH );
	Sleep( 1000 );  
	tacho_stop( MOTOR_LEFT );
	Sleep( 2000 ); 
	tacho_stop( MOTOR_RIGHT );
	Sleep( 2000 );
	
	tacho_run_forever( MOTOR_BOTH );
	while(!sensor_get_value(0, touchSensor, 0)); //Så länge touch-sensorn inte är intryckt kommer while-loopen köras
	tacho_stop( MOTOR_BOTH );

	brick_uninit();
	printf( "dying...\n" );
    	return ( 0 );
}