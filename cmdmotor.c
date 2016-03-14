/**
 * Cgi-bin and command line tool for controlling two stepper motor wheels
 * on a three wheel robot carriage.
 * Copyright (C) 2015 Jussi Salin <salinjus@gmail.com>
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
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>
#include "config.h"

/**
 * Main program
 */
int main(int argc, const char *argv[])
{
    int i, dir1, dir2, degrees, steps;
    char *get;

    // Get parameters from either cgi-bin get parameters or command line
    get = getenv("QUERY_STRING");
    if (get != NULL)
    {
        printf("Content-Type:text/plain;charset=us-ascii\n\n");
        if (sscanf(get, "deg=%i&dir1=%i&dir2=%i", &degrees, &dir1, &dir2) != 3)
        {
            printf("Invalid parameters\n");
            return 1;
        }
    }
    else if (argc != 4)
    {
        printf("Usage: motor <degrees> <motor1 direction> <motor2 direction>\n");
        printf("Example: motor 360 0 1\n");
        return 1;
    }
    else
    {
        degrees = atoi(argv[1]);
        dir1 = atoi(argv[2]);
        dir2 = atoi(argv[3]);
    }

    // Initialize Wiring PI library and pins
    wiringPiSetupGpio();

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

    return 0;
}
