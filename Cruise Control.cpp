#include <stdio.h>
#include <math.h>
#include <time.h>

// Define the maximum speed limit
#define MAX_SPEED 120

// Define the minimum and maximum acceleration values
#define MIN_ACCEL -5
#define MAX_ACCEL 5

// Define the proportional gain constant (Kp)
#define KP 1.0f

// Define the integral gain constant (Ki)
#define KI 0.1f

// Define the derivative gain constant (Kd)
#define KD 0.01f

// Define current speed, desired speed, and total distance
int speed = 0;
float target_speed = 0.0f;
float total_distance = 0.0f;

// Define error, integral, derivative, and previous time
float e = 0.0f;
float I = 0.0f;
float D = 0.0f;
clock_t previous_time = 0;

// File pointer for logging
FILE *logFile;

void print_timestamp(void) {
  // Get the current time
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  // Print timestamp and total distance
  printf("[%d-%02d-%02d %02d:%02d:%02d] Total distance: %.2f km\n",
    timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
    timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, total_distance);


	fprintf(logFile, "[%d-%02d-%02d %02d:%02d:%02d] Total distance: %.2f km\n",
        timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, total_distance);
        
        // Flush the buffer to ensure data is written immediately
  	fflush(logFile);
}

void pid_control(void) {
  // Get current time and calculate time elapsed
  clock_t current_time = clock();
  float time_elapsed = (float)(current_time - previous_time) / CLOCKS_PER_SEC;
  previous_time = current_time;

  // Calculate error
  e = target_speed - (float)speed;

  // Calculate proportional, integral, and derivative terms
  float P = KP * e;
  I = KI * e + I;
  D = KD * (e - D);

  // Calculate control output and limit it
  float u = P + I + D;
  u = fminf(fmaxf(MIN_ACCEL, u), MAX_ACCEL);

  // Print timestamp
  print_timestamp();

  // Analyze speed trend
  if (e > 0) {
    printf("Slightly increasing speed\n");
  } else if (e < 0) {
    printf("Slightly decreasing speed\n");
  } else {
    printf("Constant speed\n");
  }

  // Update speed and total distance
  speed += (int)u;
  total_distance += speed * time_elapsed/60;

  // Clamp speed to limits
  if (speed == MAX_SPEED) {
    printf("**Reached maximum speed limit**\n");
    speed = MAX_SPEED;
  } else if (speed == 0) {
    printf("**Stopped**\n");
    speed = 0;
  }
}

int main(void) {
	
	// Open the log file
  logFile = fopen("cruise_log.txt", "a"); // "a" appends to the file if it exists, or creates a new file if it doesn't
  if (logFile == NULL) {
    perror("Error opening file");
    return 1;
  }
  
  // Initialize variables
  speed = 0;
  target_speed = 60.0f;
  total_distance = 0.0f;
  previous_time = clock();

  while (1) {
    // Read user input
    printf("Enter the desired speed (0-120): ");
    scanf("%f", &target_speed);

    // Validate input
    if (target_speed >= 0 && target_speed <= MAX_SPEED) {
      // Reset error and integral
      e = 0.0f;
      I = 0.0f;

      printf("Cruise control enabled\n");
    } else {
      printf("Invalid input. Please enter a speed between 0 and 120.\n");
      continue;
    }

    // Apply PID control and display information
    pid_control();
    printf("Current speed: %d km/h", speed);
    if (e > 0) {
      printf(" (increasing)\n");
    } else if (e < 0) {
      printf(" (decreasing)\n");
    } else {
      printf("\n");
    }

    // Wait for user input
    getchar();
  }
  // Close the log file when the program exits
  fclose(logFile);
  
  return 0 ;
}

