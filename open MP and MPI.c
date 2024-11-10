#include <stdio.h>
#include <math.h>
#include <omp.h>

#define TERMS 10 // Number of terms to approximate the series expansions

// Factorial function
double factorial(int n) {
    if (n == 0 || n == 1) return 1;
    double result = 1;

    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// (a) Sine function approximation
double parallel_sin(double x) {
    double sin_x = 0.0;
    #pragma omp parallel for reduction(+:sin_x)
    for (int n = 0; n < TERMS; n++) {
        double term = pow(-1, n) * pow(x, 2*n + 1) / factorial(2*n + 1);
        sin_x += term;
    }
    return sin_x;
}

// (b) Cosine function approximation
double parallel_cos(double x) {
    double cos_x = 0.0;
    #pragma omp parallel for reduction(+:cos_x)
    for (int n = 0; n < TERMS; n++) {
        double term = pow(-1, n) * pow(x, 2*n) / factorial(2*n);
        cos_x += term;
    }
    return cos_x;
}

// (c) Exponential function approximation
double parallel_exp(double x) {
    double exp_x = 0.0;
    #pragma omp parallel for reduction(+:exp_x)
    for (int n = 0; n < TERMS; n++) {
        double term = pow(x, n) / factorial(n);
        exp_x += term;
    }
    return exp_x;
}

// (d) Check if a 5-digit number is a palindrome and calculate the sum of its digits
void check_palindrome_and_sum(int num) {
    int original_num = num;
    int reversed_num = 0, sum_digits = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        { // Calculate reversed number
            int n = num;
            while (n > 0) {
                reversed_num = reversed_num * 10 + (n % 10);
                n /= 10;
            }
        }

        #pragma omp section
        { // Calculate sum of digits
            int n = num;
            while (n > 0) {
                sum_digits += n % 10;
                n /= 10;
            }
        }
    }

    // Output results
    printf("Number: %d\n", original_num);
    printf("Sum of digits: %d\n", sum_digits);
    printf("Palindrome: %s\n", (original_num == reversed_num) ? "Yes" : "No");
}

int main() {
    double x;
    int num;

    // Get input for trigonometric and exponential calculations
    printf("Enter value for X (in radians): ");
    scanf("%lf", &x);
    printf("Approximated sin(%lf): %lf\n", x, parallel_sin(x));
    printf("Approximated cos(%lf): %lf\n", x, parallel_cos(x));
    printf("Approximated exp(%lf): %lf\n", x, parallel_exp(x));

    // Get input for palindrome and sum of digits calculation
    printf("Enter a 5-digit number: ");
    scanf("%d", &num);
    check_palindrome_and_sum(num);

    return 0;
}
