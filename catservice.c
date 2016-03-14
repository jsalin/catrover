/**
 * Cat Rover background service
 * Copyright (C) 2015 Jussi Salin <salinjus@gmail.com>
 *
 * Background process to be ran as root, which controls some services based
 * on what user choose through on the web gui through the cgi-bin backend.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "config.h"

// Location of temporary files that are flag delivered from web gui
#define CAMERA_ON "/tmp/cat_camera_on"
#define CAMERA_OFF "/tmp/cat_camera_off"
#define AUDIO_ON "/tmp/cat_audio_on"
#define AUDIO_OFF "/tmp/cat_audio_off"
#define LASER_ON "/tmp/cat_laser_on"
#define LASER_OFF "/tmp/cat_laser_off"
#define LIGHT_ON "/tmp/cat_light_on"
#define LIGHT_OFF "/tmp/cat_light_off"
#define MOTOR "/tmp/cat_motor"

char msg[100]; // Global variable for passing message that might come with flag
const size_t msglen = 100;

/**
 * Check if a flag is set, and unset it if it was set.
 * Flags are temporary files touched by various cgi-bin scripts.
 */
int flagged(char *flag_file)
{
	size_t len;
	FILE *f;

	if (access(flag_file, F_OK) != -1)
	{
		printf("Flag %s was set\n", flag_file);

		// Sleep for a little bit because the backend might still be writing
		usleep(FILE_OPEN_DELAY * 1000);

		// Get message (first line) of the flag file to global variable msg or leave it NULL if no message
                f = fopen(flag_file, "r");
                if (fgets(&msg[0], msglen, f) > 0)
		{
			printf("Flag had message: %s\n", msg);
		}
		else
		{
			msg[0]=0;
		}
                fclose(f);

		// Unset (remove) the flag
		if (remove(flag_file) != 0)
		{
			printf("Couldn't remove flag file %s, something is wrong - exiting\n", flag_file);
			exit(1);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * Execute system() call on forked background thread
 */
void system_bg(char *cmd)
{
	if (fork() == 0)
	{
		system(cmd);
		exit(0);
	}
}

/**
 * Control motors so that the droid moves
 */
void motor(char *params)
{
    int degrees, dir1, dir2, steps, i;

    if (sscanf(params, "deg=%i&dir1=%i&dir2=%i", &degrees, &dir1, &dir2) != 3)
    {
        printf("Invalid parameters: %s\n", params);
        return;
    }

    if (degrees>360*10)
    {
        printf("Degrees value is for some reason so high (%i) we are not proceeding for safety\n", degrees);
        return;
    }

    pinMode(DIR_PIN1, OUTPUT);
    pinMode(DIR_PIN2, OUTPUT);
    pinMode(STEP_PIN1, OUTPUT);
    pinMode(STEP_PIN2, OUTPUT);
    pinMode(ENABLE_PIN1, OUTPUT);
    pinMode(ENABLE_PIN2, OUTPUT);

    // Set directions and enable motor drivers
    digitalWrite(DIR_PIN1, dir1);
    digitalWrite(DIR_PIN2, dir2);
    digitalWrite(ENABLE_PIN1, LOW); // Inverse logic
    digitalWrite(ENABLE_PIN2, LOW);

    steps = degrees * STEPS_PER_DEGREE;
    printf("Rotating %i steps (%i degrees) with directions %i, %i in %ims\n", steps, degrees, dir1, dir2, STEP_INTERVAL*steps/1000);

    // Perform rotation of both motors
    for (i=0; i<steps/2; i++)
    {
        digitalWrite(STEP_PIN1, HIGH);
        digitalWrite(STEP_PIN2, HIGH);
        usleep(STEP_INTERVAL);
        digitalWrite(STEP_PIN1, LOW);
        digitalWrite(STEP_PIN2, LOW);
        usleep(STEP_INTERVAL);
    }

    // Disable motor drivers to save energy
    digitalWrite(ENABLE_PIN1, HIGH); // Inverse logic
    digitalWrite(ENABLE_PIN2, HIGH);
}

/**
 * Main program - runs indefinately unless an error occurs
 */
int main(void)
{
	int camera_time, light_time, laser_time, audio_time;
	FILE *f;
	size_t len;

	wiringPiSetupGpio();
	pinMode(LASER_PIN, OUTPUT);
	pinMode(LIGHT_PIN, OUTPUT);

	camera_time = 0;
	light_time = 0;
	laser_time = 0;
	audio_time = 0;

	printf("Polling for flags\n");

	for(;;)
	{
		if (flagged(CAMERA_ON))
		{
			if (camera_time <= 0)
			{
				system_bg(CAMERA_ON_CMD);
			}
			else
			{
				printf("Camera was already on, just restarting timer");
			}
			camera_time = CAMERA_TIME / POLLING_INTERVAL;
		}
		if (flagged(CAMERA_OFF))
		{
			system_bg(CAMERA_OFF_CMD);
			camera_time = 0;
		}
		if (flagged(AUDIO_ON))
		{
			if (audio_time <= 0)
			{
				system_bg(AUDIO_ON_CMD);
			}
			else
			{
				printf("Audio was already on, just restarting timer");
			}
			audio_time = AUDIO_TIME / POLLING_INTERVAL;
		}
		if (flagged(AUDIO_OFF))
		{
			system_bg(AUDIO_OFF_CMD);
			audio_time = 0;
		}
		if (flagged(LIGHT_ON))
		{
			digitalWrite(LIGHT_PIN, HIGH);
			light_time = LIGHT_TIME / POLLING_INTERVAL;
		}
		if (flagged(LIGHT_OFF))
		{
			digitalWrite(LIGHT_PIN, LOW);
			light_time = 0;
		}
		if (flagged(LASER_ON))
		{
			digitalWrite(LASER_PIN, HIGH);
			laser_time = LASER_TIME / POLLING_INTERVAL;
		}
		if (flagged(LASER_OFF))
		{
			digitalWrite(LASER_PIN, LOW);
			laser_time = 0;
		}
		if (flagged(MOTOR))
		{
			if (msg != NULL)
			{
				motor(msg);
			}
			else
			{
				printf("Motor flag file did not have any parameters\n");
			}
		}

		usleep(1000 * POLLING_INTERVAL);

		if (camera_time > 0)
		{
			camera_time--;
			if (camera_time <= 0)
			{
				printf("Turning off camera by timer\n");
				system_bg(CAMERA_OFF_CMD);
			}
		}
		if (laser_time > 0)
		{
			laser_time--;
			if (laser_time <= 0)
			{
				printf("Turning off laser by timer\n");
				digitalWrite(LASER_PIN, LOW);
			}
		}
		if (light_time > 0)
		{
			light_time--;
			if (light_time <= 0)
			{
				printf("Turning off light by timer\n");
				digitalWrite(LIGHT_PIN, LOW);
			}
		}
		if (audio_time > 0)
		{
			audio_time--;
			if (audio_time <= 0)
			{
				printf("Turning off audio by timer\n");
				system_bg(AUDIO_OFF_CMD);
			}
		}
	}

	return 0;
}
